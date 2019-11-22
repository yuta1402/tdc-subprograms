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
        drift_transition_prob_{ tdc.params().pass_ratio, tdc.params().drift_stddev, tdc.params().max_drift, params.num_segments },
        rec_calculations_{}
    {
    }

    Eigen::RowVectorXi SCDecoder::decode(const Eigen::RowVectorXi& z)
    {
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
        const size_t n = calc_exponent_code_length(params_.code_length);
        const int max_segment = tdc_.params().max_drift * params_.num_segments;

        long double ll = 0.0;
        for (int dn = -max_segment; dn <= max_segment; ++dn) {
            u[n][i] = ui;
            ll += calc_likelihood_rec(i, n, 0, params_.code_length, 0, dn, u, z);
        }

        return ll;
    }

    long double SCDecoder::calc_level0(const int a, const int da, const int xa, const Eigen::RowVectorXi& z)
    {
        const double real_da = static_cast<double>(da) / params_.num_segments;
        const int rounded_da = std::floor(real_da + 0.5);
        const double rest_da = real_da - rounded_da;

        const int j = a + rounded_da;

        if (j < 0 || static_cast<int>(params_.code_length) <= j) {
            return 0.5;
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
        const int j = i / 2;
        const int g = (a + b) / 2;
        const int k = 1;

        if (i % 2 == 0) {
            long double r = 0;

            for (int next_u = 0; next_u <= 1; ++next_u) {
                u[k-1][a+j] = (u[k][a+2*j] + next_u) % 2;
                const long double wb = calc_level0(a, da, u[k-1][a+j], z);
                u[k-1][g+j] = next_u;
                const long double wg = calc_level0(g, db, u[k-1][g+j], z);
                r += wb * wg * drift_transition_prob_(db, da);
            }
            // r *= 0.5;

            return r;
        }

        long double r = 0;

        u[k-1][a+j] = (u[k][a+2*j] + u[k][a+2*j+1]) % 2;
        const long double wb = calc_level0(a, da, u[k-1][a+j], z);
        u[k-1][g+j] = u[k][a+2*j+1];
        const long double wg = calc_level0(g, db, u[k-1][g+j], z);
        r += wb * wg * drift_transition_prob_(db, da);
        // r *= 0.5;

        return r;
    }

    long double SCDecoder::calc_likelihood_rec(const int i, const int k, const int a, const int b, const int da, const int db, InfoTable& u, const Eigen::RowVectorXi& z)
    {
        // TODO: Implement

        // daとdbの差による刈り込み
        // if (abs(db-da) > (1 << k)) {
        //     return 0.0;
        // }

        // 過去に同じ引数で呼び出しがあった場合は保存した結果を返す
        // const int m = a / (1 << k);
        // const auto& dp = rec_calculations_[k][m][da][db][u[k][a+i]];
        // if (dp.prev_index == i && dp.value != -1.0) {
        //     return dp.value;
        // }

        if (k == 1) {
            const long double r = calc_level1(i, a, b, da, db, u, z);
            // rec_calculations_[k][m][da][db][u[k][a+i]].prev_index = i;
            // rec_calculations_[k][m][da][db][u[k][a+i]].value = r;
            return r;
        }

        const int j = i/2;
        const int g = (a+b)/2;
        const int max_segment = tdc_.params().max_drift * params_.num_segments;

        if (i % 2 == 0) {
            long double r = 0;

            // dg0: d_{g-1}, dg1: d_{g}
            for (int dg0 = -max_segment; dg0 <= max_segment; ++dg0) {
                for (int dg1 = -max_segment; dg1 <= max_segment; ++dg1) {
                    for (int next_u = 0; next_u <= 1; ++next_u) {
                        u[k-1][a+j] = (u[k][a+2*j] + next_u) % 2;
                        const long double wb = calc_likelihood_rec(j, k-1, a, g, da, dg0, u, z);
                        u[k-1][g+j] = next_u;
                        const long double wg = calc_likelihood_rec(j, k-1, g, b, dg1, db, u, z);
                        r += wb * wg * drift_transition_prob_(dg1, dg0);
                    }
                }
            }
            // r *= 0.5;

            // rec_calculations_[k][m][da][db][u[k][a+i]].prev_index = i;
            // rec_calculations_[k][m][da][db][u[k][a+i]].value = r;
            return r;
        }

        long double r = 0;

        // dg0: d_{g-1}, dg1: d_{g}
        for (int dg0 = -max_segment; dg0 <= max_segment; ++dg0) {
            for (int dg1 = -max_segment; dg1 <= max_segment; ++dg1) {
                u[k-1][a+j] = (u[k][a+2*j] + u[k][a+2*j+1]) % 2;
                const long double wb = calc_likelihood_rec(j, k-1, a, g, da, dg0, u, z);
                u[k-1][g+j] = u[k][a+2*j+1];
                const long double wg = calc_likelihood_rec(j, k-1, g, b, dg1, db, u, z);
                r += wb * wg * drift_transition_prob_(dg1, dg0);
            }
        }
        // r *= 0.5;

        // rec_calculations_[k][m][da][db][u[k][a+i]].prev_index = i;
        // rec_calculations_[k][m][da][db][u[k][a+i]].value = r;
        return r;
    }
}
