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
        drift_transition_prob_{ tdc.params().pass_ratio, tdc.params().drift_stddev, tdc.params().max_drift, params.num_segments, params.offset_rate },
        rec_calculations_{}
    {
        const size_t n = exponent_code_length_;

        rec_calculations_.resize(n+1);

        for (size_t k = 0; k <= n; ++k) {
            const size_t pownk = std::pow(2, n-k);
            rec_calculations_[k].resize(pownk);

            for (size_t m = 0; m < pownk; ++m) {
                rec_calculations_[k][m].init(-max_segment_, max_segment_, estd::nivector<std::array<RecCalculationElement, 2>>(-max_segment_, max_segment_));
            }
        }
    }

    void SCDecoder::init()
    {
        const size_t n = exponent_code_length_;

        for (size_t k = 0; k <= n; ++k) {
            const size_t pownk = std::pow(2, n-k);
            for (size_t m = 0; m < pownk; ++m) {
                rec_calculations_[k][m].init(-max_segment_, max_segment_, estd::nivector<std::array<RecCalculationElement, 2>>(-max_segment_, max_segment_));
            }
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
                const long double ll0 = calc_likelihood(i, 0, u, z);
                const long double ll1 = calc_likelihood(i, 1, u, z);

                if (ll0 >= ll1) {
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

    long double SCDecoder::calc_likelihood(const int i, const int ui, InfoTable& u, const Eigen::RowVectorXi& z)
    {
        const size_t n = exponent_code_length_;

        long double ll = 0.0;
        for (int dn = -max_segment_; dn <= max_segment_; ++dn) {
            u[n][i] = ui;
            ll += calc_likelihood_rec(i, n, 0, params_.code_length, 0, dn, u, z);
        }

        return ll;
    }

    long double SCDecoder::calc_level0(const int a, const int da, const int xa, const Eigen::RowVectorXi& z)
    {
        const double real_da = static_cast<double>(da) / params_.num_segments;
        const int rounded_da = std::floor(real_da + a * params_.offset_rate + 0.5);
        const double rest_da = real_da + a * params_.offset_rate - rounded_da;

        const int j = a + rounded_da;

        if (j < 0 || static_cast<int>(params_.code_length) <= j) {
            return 1.0;
        }

        const double pass_ratio_bound = tdc_.params().pass_ratio * 0.5;
        if (rest_da <= -pass_ratio_bound || pass_ratio_bound <= rest_da) {
            return 0.5;
        }

        double p = 0.0;
        if (xa == z[j]) {
            p = 1.0 - tdc_.params().ps;
        } else {
            p = tdc_.params().ps;
        }

        return p;
    }

    long double SCDecoder::calc_level1(const int i, const int a, const int b, const int da, const int db, InfoTable& u, const Eigen::RowVectorXi& z)
    {
        // g = (a + b) / 2
        const int g = ((a + b) >> 1);

        if (i % 2 == 0) {
            long double r = 0;

            // u[1][a+1] = 0
            u[0][a] = (u[1][a] + 0) % 2;
            const long double wb0 = calc_level0(a, da, u[0][a], z);
            u[0][g] = 0;
            const long double wg0 = calc_level0(g, db, u[0][g], z);

            // u[1][a+1] = 1
            u[0][a] = (u[1][a] + 1) % 2;
            const long double wb1 = calc_level0(a, da, u[0][a], z);
            u[0][g] = 1;
            const long double wg1 = calc_level0(g, db, u[0][g], z);

            r = (wb0 * wg0 + wb1 * wg1) * drift_transition_prob_(db, da);
            // r *= 0.5;

            return r;
        }

        long double r = 0;

        u[0][a] = (u[1][a] + u[1][a+1]) % 2;
        const long double wb = calc_level0(a, da, u[0][a], z);
        u[0][g] = u[1][a+1];
        const long double wg = calc_level0(g, db, u[0][g], z);
        r += wb * wg * drift_transition_prob_(db, da);
        // r *= 0.5;

        return r;
    }

    long double SCDecoder::calc_likelihood_rec(const int i, const int k, const int a, const int b, const int da, const int db, InfoTable& u, const Eigen::RowVectorXi& z)
    {
        // daとdbの差による刈り込み
        // if (abs(db-da) > (1 << k)) {
        //     return 0.0;
        // }

        // m = a / 2^k
        const int m = (a >> k);

        // 過去に同じ引数で呼び出しがあった場合は保存した結果を返す
        const auto& dp = rec_calculations_[k][m][da][db][u[k][a+i]];
        if (dp.prev_index == i && dp.value != -1.0) {
            return dp.value;
        }

        if (k == 1) {
            const long double r = calc_level1(i, a, b, da, db, u, z);

            rec_calculations_[k][m][da][db][u[k][a+i]].prev_index = i;
            rec_calculations_[k][m][da][db][u[k][a+i]].value = r;
            return r;
        }

        // j = i / 2
        const int j = (i >> 1);

        // g = (a + b) / 2
        const int g = ((a + b) >> 1);

        if (i % 2 == 0) {
            long double r = 0;

            // dg0: d_{g-1}, dg1: d_{g}
            for (int dg0 = -max_segment_; dg0 <= max_segment_; ++dg0) {
                for (int dg1 = -max_segment_; dg1 <= max_segment_; ++dg1) {
                    if (drift_transition_prob_(dg1, dg0) == 0.0) {
                        continue;
                    }

                    u[k-1][a+j] = (u[k][a+2*j] + 0) % 2;
                    const long double wb0 = calc_likelihood_rec(j, k-1, a, g, da, dg0, u, z);
                    u[k-1][g+j] = 0;
                    const long double wg0 = calc_likelihood_rec(j, k-1, g, b, dg1, db, u, z);

                    u[k-1][a+j] = (u[k][a+2*j] + 1) % 2;
                    const long double wb1 = calc_likelihood_rec(j, k-1, a, g, da, dg0, u, z);
                    u[k-1][g+j] = 1;
                    const long double wg1 = calc_likelihood_rec(j, k-1, g, b, dg1, db, u, z);

                    r += (wb0 * wg0 + wb1 * wg1) * drift_transition_prob_(dg1, dg0);
                }
            }
            // r *= 0.5;

            rec_calculations_[k][m][da][db][u[k][a+i]].prev_index = i;
            rec_calculations_[k][m][da][db][u[k][a+i]].value = r;
            return r;
        }

        long double r = 0;

        // dg0: d_{g-1}, dg1: d_{g}
        for (int dg0 = -max_segment_; dg0 <= max_segment_; ++dg0) {
            for (int dg1 = -max_segment_; dg1 <= max_segment_; ++dg1) {
                if (drift_transition_prob_(dg1, dg0) == 0.0) {
                    continue;
                }

                u[k-1][a+j] = (u[k][a+2*j] + u[k][a+2*j+1]) % 2;
                const long double wb = calc_likelihood_rec(j, k-1, a, g, da, dg0, u, z);
                u[k-1][g+j] = u[k][a+2*j+1];
                const long double wg = calc_likelihood_rec(j, k-1, g, b, dg1, db, u, z);
                r += wb * wg * drift_transition_prob_(dg1, dg0);
            }
        }
        // r *= 0.5;

        rec_calculations_[k][m][da][db][u[k][a+i]].prev_index = i;
        rec_calculations_[k][m][da][db][u[k][a+i]].value = r;
        return r;
    }
}
