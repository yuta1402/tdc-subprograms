#include "test.hpp"
#include "channel/bsc.hpp"
#include "utl/ber_simulator.hpp"
#include "utl/raw_encoder.hpp"
#include "utl/raw_decoder.hpp"

void ber_simulator_test()
{
    size_t n = 10;

    utl::BERSimulatorOptions options;
    options.code_length = n;
    options.info_length = n;
    options.num_threads = 1;
    options.num_epochs = 100;
    options.min_num_error_words = 100;
    options.min_num_simulations = 100;
    options.max_num_simulations = 100;

    channel::BSC bsc(0.0);
    utl::RawEncoder encoder(n);
    utl::RawDecoder decoder;

    utl::BERSimulator<channel::BSC, utl::RawEncoder, utl::RawDecoder> simulator(options, bsc, encoder, decoder);
    auto r = simulator.simulate();

    eassert(r.simulation_count == 100, "r.simulation_count == %ld", r.simulation_count);
    eassert(r.bit_error_count == 0, "r.error_bit_count == %ld", r.bit_error_count);
    eassert(r.word_error_count == 0, "r.error_word_count == %ld", r.word_error_count);
    eassert(r.ber == 0.0, "r.ber == %f", r.ber);
    eassert(r.bler == 0.0, "r.bler == %f", r.bler);
}
