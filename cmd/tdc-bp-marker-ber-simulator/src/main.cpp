#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include "estd/random.hpp"
#include "estd/measure_time.hpp"
#include "command_line.hpp"
#include "channel/tdc.hpp"
#include "utl/marker_encoder.hpp"
#include "lcs/ldpc_ber_simulator.hpp"
#include "lcstdc/bp_marker_decoder.hpp"
#include "lcs/spmat.hpp"

int main(int argc, char* argv[])
{
    cmdl::Parser p(argc, argv);
    p.add("help", 'h', "display help", false);

    p.add("dec-file"               , 'd', "the parity check matrix", true);
    p.add("enc-file"               , 'c', "the encoder matrix", true);
    p.add("threads"                , 't', "the number of threads", true);
    p.add("epochs"                 , 'e', "the number of trials once", true);
    p.add("error-words"            , 'w', "the minimum of count of error words", true);
    p.add("seed"                   , 'r', "the seed of random device", true);
    p.add("max-simulations"        , 'm', "the maximum of simulation", true);
    p.add("frozen-bit-simulations" , 'f', "the number of simulations to determinate of frozen bits", true);

    // channel params
    p.add("ps"          , '0', "the probability of substitution error for Timing Drift Channel", true);
    p.add("pass-ratio"  , '1', "pass ratio", true);
    p.add("drift-stddev", '2', "the standard deviation of transition probability of drift (i.e. p_id)", true);

    // decoder params
    p.add("max-drift"   , '3', "the maximum of drift value", true);
    p.add("segments"    , '4', "the number of segments", true);
    p.add("iterations"  , '5', "the number of iterations of bp", true);

    if(!p.parse()) {
        std::cout << p.usage() << std::endl;
        return 1;
    }

    if (p.exist("help")) {
        std::cout << p.usage() << std::endl;
        return 0;
    }

    const std::string dec_filename          = p.get<std::string>("dec-file"         , "./data/252.252.3.252.spmat.dec");
    const std::string enc_filename          = p.get<std::string>("enc-file"         , "./data/252.252.3.252.spmat.enc");
    const size_t num_threads                = p.get<size_t>("threads"               , 12);
    const size_t num_epochs                 = p.get<size_t>("epochs"                , 100);
    const size_t min_num_error_words        = p.get<size_t>("error-words"           , 100);
    const size_t seed                       = p.get<size_t>("seed"                  , 0);
    const size_t max_num_simulations        = p.get<size_t>("max-simulations"       , 10000);
    const size_t num_frozen_bit_simulations = p.get<size_t>("frozen-bit-simulations", 1000);

    // channel params
    const double ps           = p.get<double>("ps"          , 1e-2);
    const double pass_ratio   = p.get<double>("pass-ratio"  , 1.0);
    const double drift_stddev = p.get<double>("drift-stddev", 1.0);

    // decoder params
    const int max_drift             = p.get<int>("max-drift"    , 2);
    const int num_segments          = p.get<int>("segments"     , 2);
    const size_t max_num_iterations = p.get<size_t>("iterations", 100);

    lcs::Spmat enc_spmat(enc_filename);
    lcs::Spmat dec_spmat(dec_filename);

    auto enc_matrix = enc_spmat.toMatrix();
    auto dec_matrix = dec_spmat.toSparseMatrix();

    estd::Reseed(seed);

    // メルセンヌ・ツイスタの生成する値のうち、最初の100万個を捨てる (分布が良くないらしい)
    for (int i = 0; i < 1e+6; ++i) {
        estd::Random();
    }

    std::cout << "code parameters:\n"
              // << "    code length: " << code_length << '\n'
              // << "    info length: " << info_length << '\n'
              << "channel parameters:\n"
              << "    ps: " << ps << '\n'
              << "    pass ratio: " << pass_ratio << '\n'
              << "    drift stddev: " << drift_stddev << '\n'
              << "decoder parameters:\n"
              << "    max drift: " << max_drift << '\n'
              << "    num segments: " << num_segments << '\n'
              << "    max num iterations: " << max_num_iterations << '\n'
              << "simulation parameters:\n"
              << "    seed: " << seed << '\n'
              << "    num threads: " << num_threads << '\n'
              << "    num epochs: " << num_epochs << '\n'
              << "    min num error words: " << min_num_error_words << '\n'
              << "    max num simulations: " << max_num_simulations << '\n'
              << "    num simulations (frozen bit): " << num_frozen_bit_simulations << '\n'
              << std::endl;

    channel::TDCParams tdc_params;
    tdc_params.ps = ps;
    tdc_params.pass_ratio = pass_ratio;
    tdc_params.drift_stddev = drift_stddev;
    tdc_params.max_drift = max_drift;

    channel::TDC tdc(tdc_params);

    auto marker_handler = utl::MarkerHandler::Marker01(2);

    utl::MarkerEncoder encoder(marker_handler);

    lcstdc::BPMarkerDecoder::Options decoder_options;
    decoder_options.tdc = tdc;
    decoder_options.marker_handler = marker_handler;
    decoder_options.num_segments = num_segments;
    decoder_options.parity_check_matrix = dec_matrix;
    decoder_options.max_num_iterations = max_num_iterations;

    lcstdc::BPMarkerDecoder decoder(decoder_options);

    lcs::LDPCBERSimulatorOptions options;
    options.num_threads = num_threads;
    options.max_num_simulations = max_num_simulations;
    options.min_num_error_words = min_num_error_words;
    options.encoder_matrix = enc_matrix;
    options.num_epochs = num_epochs;

    lcs::LDPCBERSimulator<channel::TDC, utl::MarkerEncoder, lcstdc::BPMarkerDecoder> simulator(options, tdc, encoder, decoder);
    simulator.simulate();

    return 0;
}
