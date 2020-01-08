#include <algorithm>
#include <unordered_set>
#include <memory>
#include <queue>
#include "pcstdc/scl_crc_decoder.hpp"

namespace
{
    size_t calc_exponent_code_length(size_t code_length)
    {
        if (code_length == 0) {
            return 0;
        }

        size_t n = 0;
        while (code_length) {
            code_length = code_length >> 1;
            ++n;
        }

        return n-1;
    }

    struct PathWrap
    {
    public:
        Eigen::RowVectorXi path;
        pcstdc::InfoTableHandler info_table;
        std::shared_ptr<pcstdc::SCDecoder> sc_decoder;

        PathWrap(const pcstdc::SCDecoderParams& decoder_params, const channel::TDC& tdc, const std::vector<int>& frozen_bits) :
            path{ Eigen::RowVectorXi::Zero(decoder_params.code_length) },
            info_table{ decoder_params.code_length },
            sc_decoder{ std::make_shared<pcstdc::SCDecoder>(decoder_params, tdc, frozen_bits) }
        {}
    };

    struct LikelihoodWrap
    {
        size_t l;
        int ui;
        long double likelihood;

        LikelihoodWrap(const size_t l_, const int ui_, const long double likelihood_) :
            l{ l_ },
            ui{ ui_ },
            likelihood{ likelihood_ }
        {}
    };

    struct PathLikelihoodWrap
    {
    public:
        int ui;
        long double likelihood;
        PathWrap path_wrap;

        PathLikelihoodWrap(const int ui_, const long double likelihood_, const PathWrap& path_wrap_) :
            ui{ ui_ },
            likelihood{ likelihood_ },
            path_wrap{ path_wrap_ }
        {}
    };

    Eigen::RowVectorXi extract_info_word(const size_t info_length, const Eigen::RowVectorXi& x, const std::vector<int>& frozen_bits)
    {
        Eigen::RowVectorXi m(info_length);

        size_t j = 0;
        for (int i = 0; i < x.size(); ++i) {
            if (frozen_bits[i]) {
                continue;
            }

            m[j] = x[i];
            ++j;
            if (j >= info_length) {
                break;
            }
        }

        return m;
    }

    inline estd::nivector<estd::nivector<std::array<long double, 2>>> generate_initial_value(const int max_segment)
    {
        const estd::nivector<estd::nivector<std::array<long double, 2>>> r(
            -max_segment, max_segment, estd::nivector<std::array<long double, 2>>(
                -max_segment, max_segment, { 0.0, 0.0 }
            )
        );

        return r;
    }
}

namespace pcstdc
{
    SCLCRCDecoder::SCLCRCDecoder(const SCDecoderParams& decoder_params, const channel::TDC& tdc, const std::vector<int>& frozen_bits, const utl::CRCHandler& crc_handler, const size_t list_size) :
        decoder_params_{ decoder_params },
        tdc_{ tdc },
        frozen_bits_{ frozen_bits },
        exponent_code_length_{ calc_exponent_code_length(decoder_params.code_length) },
        max_segment_{ tdc_.params().max_drift * decoder_params.num_segments },
        drift_transition_prob_{ tdc.params().pass_ratio, tdc.params().drift_stddev, tdc.params().max_drift, decoder_params.num_segments },
        rec_calculations_{},
        level0_calculations_{},
        crc_handler_{ crc_handler },
        list_size_{ list_size }
    {
        const size_t n = exponent_code_length_;

        rec_calculations_.resize(list_size_);

        for (size_t l = 0; l < list_size_; ++l) {
            rec_calculations_[l].resize(n+1);
            for (size_t k = 0; k <= n; ++k) {
                const size_t pownk = (1 << (n-k));
                rec_calculations_[l][k].resize(pownk);

                for (size_t m = 0; m < pownk; ++m) {
                    rec_calculations_[l][k][m].value.assign(
                        -max_segment_, max_segment_, estd::nivector<std::array<long double, 2>>(
                            -max_segment_, max_segment_, { -1.0, -1.0 }
                        )
                    );
                }
            }
        }

        const size_t code_length = decoder_params.code_length;
        level0_calculations_.resize(code_length);
        for (size_t i = 0; i < code_length; ++i) {
            level0_calculations_[i].assign(
                -max_segment_, max_segment_, { -1.0, -1.0 }
            );
        }
    }

    void SCLCRCDecoder::init()
    {
        const size_t n = exponent_code_length_;

        for (size_t l = 0; l < list_size_; ++l) {
            for (size_t k = 0; k <= n; ++k) {
                const size_t pownk = (1 << (n-k));
                for (size_t m = 0; m < pownk; ++m) {
                    rec_calculations_[l][k][m].prev_index = -1;
                    rec_calculations_[l][k][m].value.fill(estd::nivector<std::array<long double, 2>>(-max_segment_, max_segment_, { -1.0, -1.0 }));
                }
            }
        }

        const size_t code_length = decoder_params_.code_length;
        for (size_t i = 0; i < code_length; ++i) {
            level0_calculations_[i].fill({ -1.0, -1.0 });
        }
    }

    Eigen::RowVectorXi SCLCRCDecoder::decode(const Eigen::RowVectorXi& z)
    {
        // 再帰計算の初期化
        init();

        const size_t n = exponent_code_length_;
        const size_t code_length = decoder_params_.code_length;

        // std::vector<Eigen::RowVectorXi> paths(list_size_, Eigen::RowVectorXi::Zero(code_length));
        std::vector<InfoTable> u(list_size_, InfoTable(code_length));
        std::vector<bool> actives(list_size_, false);

        actives[0] = true;

        std::vector<LikelihoodWrap> candidates;

        for (size_t i = 0; i < code_length; ++i) {
            if (frozen_bits_[i]) {
                for (auto&& ul : u) {
                    ul.update(i, 0);
                }
                continue;
            }

            candidates.clear();

            const auto& ll = calc_likelihood(i, u, z);

            for (size_t l = 0; l < list_size_; ++l) {
                if (actives[l]) {
                    candidates.emplace_back(l, 0, ll[l][0]);
                    candidates.emplace_back(l, 1, ll[l][1]);
                }
            }

            std::sort(std::begin(candidates), std::end(candidates), [](const auto& c1, const auto& c2){
                return c1.likelihood > c2.likelihood;
            });

            candidates.erase(std::begin(candidates) + std::min(list_size_, candidates.size()), std::end(candidates));

            std::queue<size_t> inactive_indices;

            for (size_t l = 0; l < list_size_; ++l) {
                auto it = std::find_if(std::begin(candidates), std::end(candidates), [l](const auto& c){
                    return c.l == l;
                });

                if (it == std::end(candidates)) {
                    inactive_indices.emplace(l);
                }
            }

            std::fill(std::begin(actives), std::end(actives), false);

            for (auto&& c : candidates) {
                if (actives[c.l]) {
                    auto j = inactive_indices.front();
                    inactive_indices.pop();

                    actives[j] = true;
                    u[j] = u[c.l];
                    u[j].update(i, c.ui);

                    rec_calculations_[j] = rec_calculations_[c.l];
                    continue;
                }

                actives[c.l] = true;
                u[c.l].update(i, c.ui);
            }
        }

        // CRCで符号語と判断した場合はそれを返す
        for (size_t l = 0; l < list_size_; ++l) {
            const auto& crc_word = extract_info_word(decoder_params_.info_length + crc_handler_.num_crc_bits(), u[l].infoword(), frozen_bits_);
            if (crc_handler_.check(crc_word)) {
                const auto& m = extract_info_word(decoder_params_.info_length, u[l].infoword(), frozen_bits_);
                return m;
            }
        }

        // CRCで符号語が見つからない場合は，最後のビットの尤度が一番高い符号語を返す
        const auto& m = extract_info_word(decoder_params_.info_length, u[candidates[0].l].infoword(), frozen_bits_);
        return m;
    }

    std::vector<std::array<long double, 2>> SCLCRCDecoder::calc_likelihood(const int i, InfoTables& u, const Eigen::RowVectorXi& z)
    {
        const size_t n = exponent_code_length_;

        // const auto& w = calc_likelihood_rec(i, n, 0, decoder_params_.code_length, u, z);
        calc_likelihood_rec(i, n, 0, decoder_params_.code_length, u, z);

        std::vector<std::array<long double, 2>> ll(list_size_, { 0.0, 0.0 });
        for (size_t l = 0; l < list_size_; ++l) {
            // TODO: activeじゃないlは飛ばすべき

            const auto& w = rec_calculations_[l][n][0].value;

            for (int dn = -max_segment_; dn <= max_segment_; ++dn) {
                ll[l][0] += w[0][dn][0];
                ll[l][1] += w[0][dn][1];
            }
        }

        return ll;
    }

    std::array<long double, 2> SCLCRCDecoder::calc_level0(const int a, const int da, const Eigen::RowVectorXi& z)
    {
        const double real_da = static_cast<double>(da) / decoder_params_.num_segments;
        const int rounded_da = std::floor(real_da + 0.5);
        const double rest_da = real_da - rounded_da;

        const int j = a + rounded_da;

        if (j < 0 || static_cast<int>(decoder_params_.code_length) <= j) {
            return { 1.0, 1.0 };
        }

        const double pass_ratio_bound = tdc_.params().pass_ratio * 0.5;
        if (rest_da <= -pass_ratio_bound || pass_ratio_bound <= rest_da) {
            return { 0.5, 0.5 };
        }

        std::array<long double, 2> p;

        if (z[j] == 0) {
            p[0] = 1.0 - tdc_.params().ps;
            p[1] = tdc_.params().ps;
        } else {
            p[0] = tdc_.params().ps;
            p[1] = 1.0 - tdc_.params().ps;
        }

        return p;
    }

    estd::nivector<std::array<long double, 2>> SCLCRCDecoder::calc_level0_rec(const int a, const Eigen::RowVectorXi& z)
    {
        if (level0_calculations_[a][0][0] != -1.0) {
            return level0_calculations_[a];
        }

        estd::nivector<std::array<long double, 2>> r(
            -max_segment_, max_segment_, { 0.0, 0.0 }
        );

        for (int da = -max_segment_; da <= max_segment_; ++da) {
            r[da] = calc_level0(a, da, z);
        }

        level0_calculations_[a] = r;
        return r;
    }

    void SCLCRCDecoder::calc_level1_rec(const int i, const int a, const int b, InfoTables& u, const Eigen::RowVectorXi& z)
    {
        // m = a / 2^k
        const int m = (a >> 1);

        // g = (a + b) / 2
        const int g = ((a + b) >> 1);

        const auto& wb = calc_level0_rec(a, z);
        const auto& wg = calc_level0_rec(g, z);

        for (size_t l = 0; l < list_size_; ++l) {
            auto r = generate_initial_value(max_segment_);

            for (const auto& [da, db, dtp] : drift_transition_prob_.not_zero_range()) {
                if (i & 1) {
                    // odd-index
                    r[da][db][0] = wb[da][u[l][1][a]]   * wg[db][0] * drift_transition_prob_(db, da);
                    r[da][db][1] = wb[da][u[l][1][a]^1] * wg[db][1] * drift_transition_prob_(db, da);
                } else {
                    // even-index
                    r[da][db][0] = (wb[da][0] * wg[db][0] + wb[da][1] * wg[db][1]) * drift_transition_prob_(db, da);
                    r[da][db][1] = (wb[da][1] * wg[db][0] + wb[da][0] * wg[db][1]) * drift_transition_prob_(db, da);
                }
            }

            rec_calculations_[l][1][m].prev_index = i;
            rec_calculations_[l][1][m].value = r;
        }
    }

    void SCLCRCDecoder::calc_likelihood_rec(const int i, const int k, const int a, const int b, InfoTables& u, const Eigen::RowVectorXi& z)
    {
        // m = a / 2^k
        const int m = (a >> k);

        // 過去に同じ引数で呼び出しがあった場合は保存した結果を返す
        const auto& dp = rec_calculations_[0][k][m];
        if (dp.prev_index == i && dp.value[0][0][0] != -1.0) {
            return;
        }

        if (k == 1) {
            calc_level1_rec(i, a, b, u, z);
            return;
        }

        // j = i / 2
        const int j = (i >> 1);

        // g = (a + b) / 2
        const int g = ((a + b) >> 1);

        // const auto& wb = calc_likelihood_rec(j, k-1, a, g, u, z);
        // const auto& wg = calc_likelihood_rec(j, k-1, g, b, u, z);
        calc_likelihood_rec(j, k-1, a, g, u, z);
        calc_likelihood_rec(j, k-1, g, b, u, z);

        for (size_t l = 0; l < list_size_; ++l) {
            const auto& wb = rec_calculations_[l][k-1][(a >> (k-1))].value;
            const auto& wg = rec_calculations_[l][k-1][(g >> (k-1))].value;

            auto r = generate_initial_value(max_segment_);

            for (int da = -max_segment_; da <= max_segment_; ++da) {
                for (int db = -max_segment_; db <= max_segment_; ++db) {
                    // daとdbの差による刈り込み
                    const int max_num_transitions = drift_transition_prob_.max_num_transitions();
                    if (abs(db-da) > max_num_transitions * (1 << k)) {
                        r[da][db][0] = 0.0;
                        r[da][db][1] = 0.0;
                        continue;
                    }

                    // dg0: d_{g-1}, dg1: d_{g}
                    for (const auto& [dg0, dg1, dtp] : drift_transition_prob_.not_zero_range()) {
                        if (i & 1) {
                            // odd-index
                            r[da][db][0] += wb[da][dg0][u[l][k][a+2*j]]   * wg[dg1][db][0] * dtp;
                            r[da][db][1] += wb[da][dg0][u[l][k][a+2*j]^1] * wg[dg1][db][1] * dtp;
                        } else {
                            // even-index
                            r[da][db][0] += (wb[da][dg0][0] * wg[dg1][db][0] + wb[da][dg0][1] * wg[dg1][db][1]) * dtp;
                            r[da][db][1] += (wb[da][dg0][1] * wg[dg1][db][0] + wb[da][dg0][0] * wg[dg1][db][1]) * dtp;
                        }
                    }
                }
            }

            rec_calculations_[l][k][m].prev_index = i;
            rec_calculations_[l][k][m].value = r;
        }
        // r *= 0.5;
    }
}
