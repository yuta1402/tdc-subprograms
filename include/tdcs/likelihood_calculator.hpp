#ifndef TDCS_LIKELIHOOD_CALCULATOR_HPP
#define TDCS_LIKELIHOOD_CALCULATOR_HPP

#include <array>
#include <vector>
#include <Eigen/Core>
#include "channel/tdc.hpp"
#include "tdcs/drift_transition_prob.hpp"
#include "utl/cyclic_prior_prob.hpp"

/*!
@namespace tdcs
@brief Timing Drift Channel Subprograms
*/
namespace tdcs
{
    /*!
    @class LikelihoodCalculator
    @brief the calculator of likelihood for Timing Drift Channel
    */
    class LikelihoodCalculator
    {
    public:
        struct Params
        {
            int num_segments;
            utl::CyclicPriorProb prior_prob;
        };

    public:
        LikelihoodCalculator(const channel::TDC& tdc, const Params& params);
        ~LikelihoodCalculator() = default;

        std::vector<std::array<double, 2>> calc(const Eigen::RowVectorXi& z);
        Eigen::RowVectorXd calc_llr(const Eigen::RowVectorXi& z);

    private:
        channel::TDC tdc_;
        Params params_;
        DriftTransitionProb dtp_;
    };
}

#endif
