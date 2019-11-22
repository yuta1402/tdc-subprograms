#ifndef PCTD2CS_FROZEN_BIT_SELECTOR_HPP
#define PCTD2CS_FROZEN_BIT_SELECTOR_HPP

#include <vector>
#include <thread>
#include "channel/tdc.hpp"
#include "pcstdc/sc_decoder.hpp"

/*!
@namespace pcstdc
@brief Polar Code Subprograms for Timing Drift Channel
*/
namespace pcstdc
{
    class FrozenBitSelector
    {
    public:
        static constexpr size_t kInitialNumSimulation = 1e+5;

    public:
        FrozenBitSelector(const channel::TDC& channel, const SCDecoderParams& decoder_params, const size_t num_simulation = kInitialNumSimulation);
        ~FrozenBitSelector() = default;

        void simulate(std::vector<std::pair<size_t, double>>& capacities);
        void parallel_simulate(std::vector<std::pair<size_t, double>>& capacities, size_t num_threads = std::thread::hardware_concurrency());

        void select();
        void parallel_select(size_t num_threads = std::thread::hardware_concurrency());

        const std::vector<int>& get_frozen_bits() { return frozen_bits_; }

    private:
        const size_t code_length_;
        const size_t info_length_;
        const size_t num_simulation_;

        const channel::TDC& channel_;
        const SCDecoderParams& decoder_params_;

        std::vector<int> frozen_bits_;
    };
}

#endif
