#include <vector>
#include "estd/negative_index_vector.hpp"
#include "tdc_capacity_calculator.hpp"
#include "pcstdc/drift_transition_prob.hpp"

TDCCapacityCalculator::TDCCapacityCalculator(const Params& params, const channel::TDC& tdc) :
    params_{ params },
    tdc_{ tdc },
    max_segment_{ tdc.params().max_drift * params.num_segments },
    dtp_{ tdc.params().pass_ratio, tdc.params().drift_stddev, tdc.params().max_drift, params.num_segments }
{
}

double TDCCapacityCalculator::calculate()
{
    return 0.0;
}
