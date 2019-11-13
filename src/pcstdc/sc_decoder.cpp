#include "pcstdc/sc_decoder.hpp"

namespace pcstdc
{
    SCDecoder::SCDecoder(const SCDecoderParams& params, const channel::TDC& tdc) :
        params_{ params },
        tdc_{ tdc },
        drift_transition_prob_{ tdc.params().pass_ratio, tdc.params().drift_stddev, tdc.params().max_drift, params.num_segments },
        rec_calculations_{}
    {
    }

    long double SCDecoder::calc_level0(const int a, const int da, const int db, const int xi, const Eigen::RowVectorXi& y)
    {
        // TODO: Implement
        return 0.0;
    }

    long double SCDecoder::calc_likelihood_rec(const int i, const int k, const int a, const int b, const int da, const int db, InfoTable& u, const Eigen::RowVectorXi& y)
    {
        // TODO: Implement

        // daとdbの差による刈り込み
        if (abs(db-da) > (1 << k)) {
            return 0.0;
        }

        // 過去に同じ引数で呼び出しがあった場合は保存した結果を返す
        const int m = a / (1 << k);
        const auto& dp = rec_calculations_[k][m][da][db][u[k][a+i]];
        if (dp.prev_index == i && dp.value != -1.0) {
            return dp.value;
        }

        // calc_all_level0で計算済みなので呼ばれることはないが念の為記述
        if (k == 0) {
            const long double r = calc_level0(a, da, db, u[0][a], y);
            rec_calculations_[k][m][da][db][u[k][a+i]].prev_index = i;
            rec_calculations_[k][m][da][db][u[k][a+i]].value = r;
            return r;
        }

        const int j = i/2;
        const int g = (a+b)/2;
        const int max_d = tdc_.params().max_drift;

        if (i % 2 == 0) {
            long double r = 0;

            for (int dg = -2*max_d; dg <= 2*max_d; ++dg) {
                for (int next_u = 0; next_u <= 1; ++next_u) {
                    u[k-1][a+j] = (u[k][a+2*j] + next_u) % 2;
                    const long double wb = calc_likelihood_rec(j, k-1, a, g, da, dg, u, y);
                    u[k-1][g+j] = next_u;
                    const long double wg = calc_likelihood_rec(j, k-1, g, b, dg, db, u, y);
                    r += wb * wg;
                }
            }
            // r *= 0.5;

            rec_calculations_[k][m][da][db][u[k][a+i]].prev_index = i;
            rec_calculations_[k][m][da][db][u[k][a+i]].value = r;
            return r;
        }

        long double r = 0;

        for (int dg = -2*max_d; dg <= 2*max_d; ++dg) {
            u[k-1][a+j] = (u[k][a+2*j] + u[k][a+2*j+1]) % 2;
            const long double wb = calc_likelihood_rec(j, k-1, a, g, da, dg, u, y);
            u[k-1][g+j] = u[k][a+2*j+1];
            const long double wg = calc_likelihood_rec(j, k-1, g, b, dg, db, u, y);
            r += wb * wg;
        }
        // r *= 0.5;

        rec_calculations_[k][m][da][db][u[k][a+i]].prev_index = i;
        rec_calculations_[k][m][da][db][u[k][a+i]].value = r;
        return r;
    }
}
