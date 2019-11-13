#ifndef PCSTDC_SC_DECODER_HPP
#define PCSTDC_SC_DECODER_HPP

#include "channel/tdc.hpp"
#include "pcstdc/drift_transition_prob.hpp"

/*!
@namespace pcstdc
@brief Polar Code Subprograms for Timing Drift Channel
*/
namespace pcstdc
{
    struct SCDecoderParams
    {
        size_t code_length;
        size_t info_length;

        int num_segments;
    };

    /*!
    @class SCDecoder
    @brief Successive Cancellation Decoder
    */
    class SCDecoder
    {
    public:
        SCDecoder(const SCDecoderParams& params, const channel::TDC& tdc);
        ~SCDecoder() = default;

        // TODO: Implement

    private:
        SCDecoderParams params_;
        channel::TDC tdc_;
        DriftTransitionProb drift_transition_prob_;
    };
}

#endif
