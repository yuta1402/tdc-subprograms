#include "pcstdc/sc_decoder.hpp"

namespace pcstdc
{
    SCDecoder::SCDecoder(const SCDecoderParams& params, const channel::TDC& tdc) :
        params_{ params },
        tdc_{ tdc },
        drift_transition_prob_{ tdc.params().pass_ratio, tdc.params().drift_stddev, tdc.params().max_drift, params.num_segments }
    {
    }
}
