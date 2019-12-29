#ifndef FROZEN_BIT_ANALYZER_HPP
#define FROZEN_BIT_ANALYZER_HPP

#include <thread>
#include "channel/tdc.hpp"
#include "pcstdc/sc_decoder.hpp"

class FrozeBitAnalyzer
{
public:
    static constexpr size_t kInitialNumSimulation = 1e+5;
    static constexpr size_t kInitialNumEpoch = 1e+2;

public:
    FrozeBitAnalyzer(const size_t code_length, const size_t info_length, const channel::TDC& channel, const pcstdc::SCDecoderParams& decoder_params, const size_t num_simulation = kInitialNumSimulation, const size_t num_epoch = kInitialNumEpoch);
    ~FrozeBitAnalyzer() = default;

    void step();
    void parallel_step(const size_t num_threads = std::thread::hardware_concurrency());

    void analyze();
    void parallel_analyze(const size_t num_threads = std::thread::hardware_concurrency());

private:
    const size_t code_length_;
    const size_t info_length_;
    const size_t num_simulation_;
    const size_t num_epoch_;

    const channel::TDC& channel_;
    const pcstdc::SCDecoderParams& decoder_params_;

    size_t simulation_count_;
    std::vector<std::pair<size_t, long double>> sum_capacities_;
    std::vector<size_t> error_bit_counts_;
};

#endif
