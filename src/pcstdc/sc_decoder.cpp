#include "pcstdc/sc_decoder.hpp"

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
}

namespace pcstdc
{
    SCDecoder::SCDecoder(const SCDecoderParams& params, const channel::TDC& tdc, const std::vector<int>& frozen_bits) :
        params_{ params },
        tdc_{ tdc },
        frozen_bits_{ frozen_bits },
        exponent_code_length_{ calc_exponent_code_length(params.code_length) },
        max_segment_{ tdc_.params().max_drift * params_.num_segments },
        drift_transition_prob_{ tdc.params().pass_ratio, tdc.params().drift_stddev, tdc.params().max_drift, params.num_segments },
        rec_calculations_{},
        level0_calculations_{}
    {
        const size_t n = exponent_code_length_;

        rec_calculations_.resize(n+1);

        for (size_t k = 0; k <= n; ++k) {
            const size_t pownk = (1 << (n-k));
            rec_calculations_[k].resize(pownk);

            for (size_t m = 0; m < pownk; ++m) {
                rec_calculations_[k][m].assign(-max_segment_, max_segment_, estd::nivector<RecCalculationElement>(-max_segment_, max_segment_));
            }
        }

        level0_calculations_.resize(params_.code_length);
        for (size_t i = 0; i < params_.code_length; ++i) {
            level0_calculations_[i].assign(-max_segment_, max_segment_, { -1.0, -1.0 });
        }
    }

    void SCDecoder::init()
    {
        const size_t n = exponent_code_length_;
        RecCalculationElement initial_value;
        initial_value.prev_index = -1;
        initial_value.value = { -1.0, -1.0 };

        for (size_t k = 0; k <= n; ++k) {
            const size_t pownk = (1 << (n-k));
            for (size_t m = 0; m < pownk; ++m) {
                rec_calculations_[k][m].fill(estd::nivector<RecCalculationElement>(-max_segment_, max_segment_, initial_value));
            }
        }

        for (size_t i = 0; i < params_.code_length; ++i) {
            level0_calculations_[i].fill({ -1.0, -1.0 });
        }
    }

    Eigen::RowVectorXi SCDecoder::decode(const Eigen::RowVectorXi& z)
    {
        // 再帰計算の初期化
        init();

        const size_t code_length = params_.code_length;
        const size_t info_length = params_.info_length;

        Eigen::RowVectorXi x(code_length);
        InfoTable u(code_length);

        for (size_t i = 0; i < code_length; ++i) {
            if (frozen_bits_[i]) {
                x[i] = 0;
            } else {
                const auto& ll = calc_likelihood(i, u, z);

                if (ll[0] >= ll[1]) {
                    x[i] = 0;
                } else {
                    x[i] = 1;
                }
            }

            u.update(i, x[i]);
        }

        Eigen::RowVectorXi m(info_length);
        int j = 0;
        for (size_t i = 0; i < code_length; ++i) {
            if (!frozen_bits_[i]) {
                m[j] = x[i];
                ++j;
            }
        }

        return m;
    }

    std::array<long double, 2> SCDecoder::calc_likelihood(const int i, InfoTable& u, const Eigen::RowVectorXi& z)
    {
        const size_t n = exponent_code_length_;

        std::array<long double, 2> ll{ 0.0, 0.0 };
        for (int dn = -max_segment_; dn <= max_segment_; ++dn) {
            const auto& w = calc_likelihood_rec(i, n, 0, params_.code_length, 0, dn, u, z);
            ll[0] += w[0];
            ll[1] += w[1];
        }

        return ll;
    }

    std::array<long double, 2> SCDecoder::calc_level0(const int a, const int da, const Eigen::RowVectorXi& z)
    {
        const double real_da = static_cast<double>(da) / params_.num_segments;
        const int rounded_da = std::floor(real_da + 0.5);
        const double rest_da = real_da - rounded_da;

        const int j = a + rounded_da;

        if (j < 0 || static_cast<int>(params_.code_length) <= j) {
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

    std::array<long double, 2> SCDecoder::calc_level0_rec(const int a, const int da, const Eigen::RowVectorXi& z)
    {
        if (level0_calculations_[a][da][0] != -1.0) {
            return level0_calculations_[a][da];
        }

        const auto& r = calc_level0(a, da, z);
        level0_calculations_[a][da] = r;
        return r;
    }

    std::array<long double, 2> SCDecoder::calc_level1_rec(const int i, const int a, const int b, const int da, const int db, InfoTable& u, const Eigen::RowVectorXi& z)
    {
        // g = (a + b) / 2
        const int g = ((a + b) >> 1);

        std::array<long double, 2> r;
        const auto& wb = calc_level0_rec(a, da, z);
        const auto& wg = calc_level0_rec(g, db, z);

        if (i & 1) {
            // odd-index
            r[0] = wb[u[1][a]]   * wg[0] * drift_transition_prob_(db, da);
            r[1] = wb[u[1][a]^1] * wg[1] * drift_transition_prob_(db, da);
        } else {
            // even-index
            r[0] = (wb[0] * wg[0] + wb[1] * wg[1]) * drift_transition_prob_(db, da);
            r[1] = (wb[1] * wg[0] + wb[0] * wg[1]) * drift_transition_prob_(db, da);
        }

        return r;
    }

    std::array<long double, 2> SCDecoder::calc_likelihood_rec(const int i, const int k, const int a, const int b, const int da, const int db, InfoTable& u, const Eigen::RowVectorXi& z)
    {
        // daとdbの差による刈り込み
        const int max_num_transitions = drift_transition_prob_.max_num_transitions();
        if (abs(db-da) > max_num_transitions * (1 << k)) {
            return { 0.0, 0.0 };
        }

        // m = a / 2^k
        const int m = (a >> k);

        // 過去に同じ引数で呼び出しがあった場合は保存した結果を返す
        const auto& dp = rec_calculations_[k][m][da][db];
        if (dp.prev_index == i && dp.value[0] != -1.0) {
            return dp.value;
        }

        if (k == 1) {
            const auto& r = calc_level1_rec(i, a, b, da, db, u, z);
            rec_calculations_[k][m][da][db].prev_index = i;
            rec_calculations_[k][m][da][db].value = r;
            return r;
        }

        // j = i / 2
        const int j = (i >> 1);

        // g = (a + b) / 2
        const int g = ((a + b) >> 1);

        std::array<long double, 2> r{ 0.0, 0.0 };

        // dg0: d_{g-1}, dg1: d_{g}
        for (const auto& [dg0, dg1, dtp] : drift_transition_prob_.not_zero_range()) {
            const auto& wb = calc_likelihood_rec(j, k-1, a, g, da, dg0, u, z);
            const auto& wg = calc_likelihood_rec(j, k-1, g, b, dg1, db, u, z);

            if (i & 1) {
                // odd-index
                r[0] += wb[u[k][a+2*j]]   * wg[0] * dtp;
                r[1] += wb[u[k][a+2*j]^1] * wg[1] * dtp;
            } else {
                // even-index
                r[0] += (wb[0] * wg[0] + wb[1] * wg[1]) * dtp;
                r[1] += (wb[1] * wg[0] + wb[0] * wg[1]) * dtp;
            }
        }
        // r *= 0.5;

        rec_calculations_[k][m][da][db].prev_index = i;
        rec_calculations_[k][m][da][db].value = r;
        return r;
    }
}
