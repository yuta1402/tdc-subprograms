#ifndef TDC_CAPCITY_CALCULATOR_HPP
#define TDC_CAPCITY_CALCULATOR_HPP

#include "channel/tdc.hpp"
#include "pcstdc/drift_transition_prob.hpp"

class TDCCapacityCalculator
{
public:
    struct Params
    {
        size_t code_length;
        size_t num_simulations;
        int num_segments;
    };

public:
    TDCCapacityCalculator(const Params& params, const channel::TDC& tdc);
    ~TDCCapacityCalculator() = default;

    double calculate();

private:
    Params params_;
    channel::TDC tdc_;

    const int max_segment_;
    pcstdc::DriftTransitionProb dtp_;
};

#endif
