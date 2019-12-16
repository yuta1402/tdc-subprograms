#ifndef PCSTDC_DRIFT_TRANSITION_PROB_HPP
#define PCSTDC_DRIFT_TRANSITION_PROB_HPP

#include <string>
#include "estd/negative_index_vector.hpp"

/*!
@namespace pcstdc
@brief Polar Code Subprograms for Timing Drift Channel
*/
namespace pcstdc
{
    /*!
    @class DriftTransitionProb
    @brief the handler of transition probability of drift
    */
    class DriftTransitionProb
    {
    public:
        using container_type = estd::nivector<estd::nivector<double>>;

    public:
        DriftTransitionProb(const double pass_ratio, const double drift_stddev, const int max_drift, const int num_segments, const double offset_rate, const std::string& prob_table_dir = "prob_table/");
        ~DriftTransitionProb() = default;

        double operator()(int next_segment, int current_segment) const;

    private:
        int max_segment_;

        std::string prob_table_dir_;

        container_type probs_;
    };
}

#endif
