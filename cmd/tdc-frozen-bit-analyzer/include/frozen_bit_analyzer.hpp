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

    bool read_cache();
    bool write_cache();

private:
    const size_t code_length_;
    const size_t info_length_;
    const size_t num_simulation_;
    const size_t num_epoch_;

    const channel::TDC& channel_;
    const pcstdc::SCDecoderParams& decoder_params_;

    size_t simulation_count_;
    std::vector<long double> average_capacities_;
    std::vector<size_t> error_bit_counts_;
    std::vector<int> prev_frozen_bits_;

    std::string cache_filename_;
};

#endif
