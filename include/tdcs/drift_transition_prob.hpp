#ifndef TDCS_DRIFT_TRANSITION_PROB_HPP
#define TDCS_DRIFT_TRANSITION_PROB_HPP

#include <string>
#include <tuple>
#include "estd/negative_index_vector.hpp"

/*!
@namespace tdcs
@brief Timing Drift Channel Subprograms
*/
namespace tdcs
{
    /*!
    @class DriftTransitionProb
    @brief the handler of transition probability of drift
    */
    class DriftTransitionProb
    {
    public:
        using container_type = estd::nivector<estd::nivector<double>>;

        using range_type           = std::tuple<int, int, double>;
        using range_container_type = std::vector<range_type>;

    public:
        DriftTransitionProb(const double pass_ratio, const double drift_stddev, const int max_drift, const int num_segments, const std::string& prob_table_dir = "prob_table/");
        ~DriftTransitionProb() = default;

        double operator()(int next_segment, int current_segment) const;

        // 確率が0でないときの(curr_seg, next_seg, prob)の一覧
        range_container_type not_zero_range() const { return not_zero_range_data_; }

    private:
        int max_segment_;

        std::string prob_table_dir_;

        container_type probs_;
        range_container_type not_zero_range_data_;
    };
}

#endif
