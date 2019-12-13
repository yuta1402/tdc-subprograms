#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include "estd/random.hpp"
#include "estd/measure_time.hpp"
#include "command_line.hpp"
#include "channel/tdc.hpp"
#include "pcstdc/polar_encoder.hpp"
#include "pcstdc/sc_decoder.hpp"
#include "pcstdc/frozen_bit_selector.hpp"

int main(int argc, char* argv[])
{
    cmdl::Parser p(argc, argv);
    p.add("help", 'h', "display help", false);

    p.add("code-length"            , 'n', "the length of codeword", true);
    p.add("info-length"            , 'k', "the length of information word", true);
    p.add("threads"                , 't', "the number of threads", true);
    p.add("epochs"                 , 'e', "the number of trials once", true);
    p.add("seed"                   , 'r', "the seed of random device", true);
    p.add("frozen-bit-simulations" , 'f', "the number of simulations to determinate of frozen bits", true);

    // channel params
    p.add("ps"          , '0', "the probability of substitution error for Timing Drift Channel", true);
    p.add("pass-ratio"  , '1', "pass ratio", true);
    p.add("drift-stddev", '2', "the standard deviation of transition probability of drift (i.e. p_id)", true);

    // decoder params
    p.add("max-drift"   , '3', "the maximum of drift value", true);
    p.add("segments"    , '4', "the number of segments", true);
    p.add("offset-rate" , '5', "offset rate", true);

    if(!p.parse()) {
        std::cout << p.usage() << std::endl;
        return 1;
    }

    if (p.exist("help")) {
        std::cout << p.usage() << std::endl;
        return 0;
    }

    const size_t code_length                = p.get<size_t>("code-length"           , 1024);
    const size_t info_length                = p.get<size_t>("info-length"           , 1024);
    const size_t num_threads                = p.get<size_t>("threads"               , 12);
    const size_t num_epochs                 = p.get<size_t>("epochs"                , 100);
    const size_t seed                       = p.get<size_t>("seed"                  , 0);
    const size_t num_frozen_bit_simulations = p.get<size_t>("frozen-bit-simulations", 1000);

    // channel params
    const double ps           = p.get<double>("ps"          , 1e-2);
    const double pass_ratio   = p.get<double>("pass-ratio"  , 1.0);
    const double drift_stddev = p.get<double>("drift-stddev", 1.0);

    // decoder params
    const int max_drift    = p.get<int>("max-drift", 2);
    const int num_segments = p.get<int>("segments" , 2);
    const double offset_rate = p.get<double>("offset-rate", 0.0);

    estd::Reseed(seed);

    // メルセンヌ・ツイスタの生成する値のうち、最初の100万個を捨てる (分布が良くないらしい)
    for (int i = 0; i < 1e+6; ++i) {
        estd::Random();
    }

    std::cout << "code parameters:\n"
              << "    code length: " << code_length << '\n'
              << "    info length: " << info_length << '\n'
              << "channel parameters:\n"
              << "    ps: " << ps << '\n'
              << "    pass ratio: " << pass_ratio << '\n'
              << "    drift stddev: " << drift_stddev << '\n'
              << "decoder parameters:\n"
              << "    max drift: " << max_drift << '\n'
              << "    num segments: " << num_segments << '\n'
              << "    offset rate: " << offset_rate << '\n'
              << "simulation parameters:\n"
              << "    num threads: " << num_threads << '\n'
              << "    num epochs: " << num_epochs << '\n'
              << "    num simulations (frozen bit): " << num_frozen_bit_simulations << '\n'
              << std::endl;

    channel::TDCParams tdc_params;
    tdc_params.ps = ps;
    tdc_params.pass_ratio = pass_ratio;
    tdc_params.drift_stddev = drift_stddev;
    tdc_params.max_drift = max_drift;
    tdc_params.offset_rate = offset_rate;

    channel::TDC tdc(tdc_params);

    pcstdc::SCDecoderParams decoder_params;
    decoder_params.code_length = code_length;
    decoder_params.info_length = info_length;
    decoder_params.num_segments = num_segments;
    decoder_params.offset_rate = offset_rate;

    pcstdc::FrozenBitSelector selector(tdc, decoder_params, num_frozen_bit_simulations);
    {
        estd::measure_time t("select frozen bits");
        selector.parallel_select(num_threads);
    }
    const auto& frozen_bits = selector.get_frozen_bits();

    pcstdc::PolarEncoder encoder(code_length, code_length, frozen_bits);
    pcstdc::SCDecoder decoder(decoder_params, tdc, frozen_bits);

    Eigen::RowVectorXi m(info_length);
    for (int i = 0; i < m.size(); ++i) {
        m[i] = estd::Random(0, 1);
    }

    Eigen::RowVectorXi x, y, m_hat;

    {
        estd::measure_time t("encode");
        x = encoder.encode(m);
    }

    {
        estd::measure_time t("send to channel");
        y = tdc.send(x);
    }

    {
        estd::measure_time t("decode");
        m_hat = decoder.decode(y);
    }

    Eigen::RowVectorXi raw_e = m + y;
    for (int i = 0; i < raw_e.size(); ++i) {
        raw_e[i] = raw_e[i] % 2;
    }

    Eigen::RowVectorXi e = m + m_hat;
    for (int i = 0; i < e.size(); ++i) {
        e[i] = e[i] % 2;
    }

    std::cout << "raw error rate: " << static_cast<double>(raw_e.sum())/code_length << std::endl;
    std::cout << "error rate: " << static_cast<double>(e.sum())/code_length << std::endl;

    return 0;
}
