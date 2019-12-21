#include "estd/negative_index_vector.hpp"
#include "tdcs/likelihood_calculator.hpp"

namespace
{
    double calc_gk(const int k, const int dk, const int xk, const Eigen::RowVectorXi& z, const channel::TDC& tdc, const int num_segments)
    {
        const double real_dk = static_cast<double>(dk) / num_segments;
        const int rounded_dk = std::floor(real_dk + 0.5);
        const double rest_dk = real_dk - rounded_dk;

        const int j = k + rounded_dk;
        const int code_length = z.size();

        if (j < 0 || code_length <= j) {
            return 1.0;
        }

        const double pass_ratio_bound = tdc.params().pass_ratio * 0.5;
        if (rest_dk <= -pass_ratio_bound || pass_ratio_bound <= rest_dk) {
            return 0.5;
        }

        double p = 0.0;
        if (xk == z[j]) {
            p = 1.0 - tdc.params().ps;
        } else {
            p = tdc.params().ps;
        }

        return p;
    }
}


namespace tdcs
{
    LikelihoodCalculator::LikelihoodCalculator(const channel::TDC& tdc, const Params& params) :
        tdc_{ tdc },
        params_{ params },
        dtp_{ tdc.params().pass_ratio, tdc.params().drift_stddev, tdc.params().max_drift, params.num_segments }
    {
    }

    std::vector<std::array<double, 2>> LikelihoodCalculator::calc(const Eigen::RowVectorXi& z)
    {
        using Message = std::vector<estd::nivector<double>>;

        const size_t code_length = z.size();
        const int max_segment = tdc_.params().max_drift * params_.num_segments;

        // upwards calculation
        Message upwards(
            code_length, estd::nivector<double>(-max_segment, max_segment, 0.0)
        );

        for (size_t k = 0; k < code_length; ++k) {
            for (int dk = -max_segment; dk <= max_segment ; ++dk) {
                const double gk0 = calc_gk(k, dk, 0, z, tdc_, params_.num_segments);
                const double gk1 = calc_gk(k, dk, 1, z, tdc_, params_.num_segments);
                upwards[k][dk] = params_.prior_prob[k][0]*gk0 + params_.prior_prob[k][1]*gk1;
            }
        }

        // forwards calculation
        Message forwards(
            code_length, estd::nivector<double>(-max_segment, max_segment, 0.0)
        );

        forwards[0][0] = 1.0;

        for (size_t k = 1; k < code_length; ++k) {
            double sum = 0.0;

            for (int dk = -max_segment; dk <= max_segment; ++dk) {
                // dj represents d_{k-1}
                for (int dj = -max_segment; dj <= max_segment; ++dj) {
                    forwards[k][dk] += forwards[k-1][dj] * dtp_(dk, dj) * upwards[k-1][dj];
                }

                sum += forwards[k][dk];
            }

            // normalization
            if (sum != 0.0) {
                double c = 1.0 / sum;
                for (int dk = -max_segment; dk <= max_segment; ++dk) {
                    forwards[k][dk] *= c;
                }
            }
        }

        // backwards calculation
        Message backwards(
            code_length, estd::nivector<double>(-max_segment, max_segment, 0.0)
        );

        for (int dn = -max_segment; dn <= max_segment; ++dn) {
            backwards[code_length-1][dn] = 1.0;
        }

        for (int k = code_length-2; k >= 0; --k) {
            double sum = 0.0;

            for (int dk = -max_segment; dk <= max_segment; ++dk) {
                // dl represents d_{k+1}
                for (int dl = -max_segment; dl <= max_segment; ++dl) {
                    backwards[k][dk] += backwards[k+1][dl] * dtp_(dl, dk) * upwards[k+1][dl];
                }

                sum += backwards[k][dk];
            }

            // normalization
            if (sum != 0.0) {
                double c = 1.0 / sum;
                for (int dk = -max_segment; dk <= max_segment; ++dk) {
                    backwards[k][dk] *= c;
                }
            }
        }

        // likelihood calculation
        std::vector<std::array<double, 2>> ll(code_length, { 0.0, 0.0 } );

        for (size_t k = 0; k < code_length; ++k) {
            for (int dk = -max_segment; dk <= max_segment; ++dk) {
                const double gk0 = calc_gk(k, dk, 0, z, tdc_, params_.num_segments);
                const double gk1 = calc_gk(k, dk, 1, z, tdc_, params_.num_segments);

                ll[k][0] += gk0 * forwards[k][dk] * backwards[k][dk];
                ll[k][1] += gk1 * forwards[k][dk] * backwards[k][dk];
            }

            double sum = ll[k][0] + ll[k][1];
            if (sum != 0.0) {
                double c = 1.0 / sum;
                ll[k][0] *= c;
                ll[k][1] *= c;
            }
        }

        return ll;
    }
}
