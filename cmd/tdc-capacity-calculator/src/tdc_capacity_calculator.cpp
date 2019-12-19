#include <vector>
#include "estd/random.hpp"
#include "estd/parallel.hpp"
#include "estd/negative_index_vector.hpp"
#include "tdc_capacity_calculator.hpp"
#include "pcstdc/drift_transition_prob.hpp"

namespace
{
    double calc_gk(const int k, const int dk, const int xk, const Eigen::RowVectorXi& z, const channel::TDC& tdc, const TDCCapacityCalculator::Params& params)
    {
        const double real_dk = static_cast<double>(dk) / params.num_segments;
        const int rounded_dk = std::floor(real_dk + 0.5);
        const double rest_dk = real_dk - rounded_dk;

        const int j = k + rounded_dk;

        if (j < 0 || static_cast<int>(params.code_length) <= j) {
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

    std::vector<double> calc_lambda(const size_t code_length, const int max_segment, const pcstdc::DriftTransitionProb& dtp, const std::vector<estd::nivector<double>>& upwards)
    {
        // sumが0の場合はlambdaがinfなので、予めinfで初期化しておく
        std::vector<double> lambda(code_length, std::numeric_limits<double>::infinity());

        std::vector<estd::nivector<double>> mu(
            code_length, estd::nivector<double>(-max_segment, max_segment)
        );

        mu[0][0] = 0.5 * upwards[0][0];

        // normalization
        lambda[0] = 1.0 / mu[0][0];
        mu[0][0] = 1.0;

        for (size_t k = 1; k < code_length; ++k) {
            double sum = 0.0;

            for (int dk = -max_segment; dk <= max_segment; ++dk) {
                double prev_sum = 0.0;
                // dj represents d_{k-1}
                for (int dj = -max_segment; dj <= max_segment; ++dj) {
                    prev_sum += mu[k-1][dj] * dtp(dk, dj);
                }

                mu[k][dk] = 0.5 * prev_sum * upwards[k][dk];
                sum += mu[k][dk];
            }

            // normalization
            if (sum != 0.0) {
                lambda[k] = 1.0 / sum;
                for (int dk = -max_segment; dk <= max_segment; ++dk) {
                    mu[k][dk] *= lambda[k];
                }
            }
        }

        return lambda;
    }

    double calc_logp(const std::vector<double>& lambda)
    {
        double logp = 0.0;
        for (size_t k = 0; k < lambda.size(); ++k) {
            logp += (-std::log2(lambda[k]));
        }

        return logp;
    }

    std::vector<estd::nivector<double>> calc_z_upwards(const int max_segment, const TDCCapacityCalculator::Params& params, const channel::TDC& tdc, const Eigen::RowVectorXi& z)
    {
        std::vector<estd::nivector<double>> upwards(
            params.code_length, estd::nivector<double>(-max_segment, max_segment)
        );

        for (size_t k = 0; k < params.code_length; ++k) {
            for (int dk = -max_segment; dk <= max_segment ; ++dk) {
                const double gk0 = calc_gk(k, dk, 0, z, tdc, params);
                const double gk1 = calc_gk(k, dk, 1, z, tdc, params);
                upwards[k][dk] = gk0 + gk1;
            }
        }

        return upwards;
    }

    std::vector<estd::nivector<double>> calc_xz_upwards(const int max_segment, const TDCCapacityCalculator::Params& params, const channel::TDC& tdc, const Eigen::RowVectorXi& x, const Eigen::RowVectorXi& z)
    {
        std::vector<estd::nivector<double>> upwards(
            params.code_length, estd::nivector<double>(-max_segment, max_segment)
        );

        for (size_t k = 0; k < params.code_length; ++k) {
            for (int dk = -max_segment; dk <= max_segment ; ++dk) {
                upwards[k][dk] = calc_gk(k, dk, x[k], z, tdc, params);
            }
        }

        return upwards;
    }
}

TDCCapacityCalculator::TDCCapacityCalculator(const Params& params, const channel::TDC& tdc) :
    params_{ params },
    tdc_{ tdc },
    max_segment_{ tdc.params().max_drift * params.num_segments },
    dtp_{ tdc.params().pass_ratio, tdc.params().drift_stddev, tdc.params().max_drift, params.num_segments }
{
}

double TDCCapacityCalculator::parallel_calculate(const size_t num_threads)
{
    std::vector<double> capacities(params_.num_simulations);

    estd::parallel_for_each_with_reseed(capacities, [this](auto&& c){
        Eigen::RowVectorXi x(params_.code_length);
        for (int i = 0; i < x.size(); ++i) {
            x[i] = estd::Random(0, 1);
        }
        const auto& z = tdc_.send(x);

        double logpz;
        {
            const auto& upwards = calc_z_upwards(max_segment_, params_, tdc_, z);
            const auto& lambda = calc_lambda(params_.code_length, max_segment_, dtp_, upwards);
            logpz = calc_logp(lambda);
        }

        double logpxz;
        {
            const auto& upwards = calc_xz_upwards(max_segment_, params_, tdc_, x, z);
            const auto& lambda = calc_lambda(params_.code_length, max_segment_, dtp_, upwards);
            logpxz = calc_logp(lambda);
        }

        c = 1.0 - logpz/params_.code_length + logpxz/params_.code_length;
        if (c < 0.0) {
            c = 0.0;
        }
    }, num_threads);

    double sum = 0.0;
    for (const auto& c : capacities) {
        sum += c;
    }

    const double c = sum / capacities.size();
    return c;
}
