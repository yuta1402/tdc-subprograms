#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include "estd/random.hpp"
#include "estd/range.hpp"
#include "command_line.hpp"
#include "channel/tdc.hpp"
#include "pcstdc/polar_encoder.hpp"
#include "tdc_capacity_calculator.hpp"

int main(int argc, char* argv[])
{
    cmdl::Parser p(argc, argv);
    p.add("help", 'h', "display help", false);

    p.add("code-length"            , 'n', "the length of codeword", true);
    p.add("threads"                , 't', "the number of threads", true);
    p.add("seed"                   , 'r', "the seed of random device", true);
    p.add("simulations"            , 'm', "the number of simulations", true);

    // channel params
    p.add("ps"          , '0', "the probability of substitution error for Timing Drift Channel", true);
    p.add("pass-ratio"  , '1', "pass ratio", true);
    p.add("drift-stddev", '2', "the standard deviation of transition probability of drift (i.e. p_id)", true);
    p.add("max-drift"   , '3', "the maximum of drift value", true);

    // simulation params
    p.add("segments"    , '5', "the number of segments", true);

    if(!p.parse()) {
        std::cout << p.usage() << std::endl;
        return 1;
    }

    if (p.exist("help")) {
        std::cout << p.usage() << std::endl;
        return 0;
    }

    const size_t code_length     = p.get<size_t>("code-length"    , 1024);
    const size_t num_threads     = p.get<size_t>("threads"        , 12);
    const size_t num_simulations = p.get<size_t>("simulations", 1000);
    const size_t seed            = p.get<size_t>("seed"           , 0);

    // channel params
    const double ps           = p.get<double>("ps"          , 1e-2);
    const double pass_ratio   = p.get<double>("pass-ratio"  , 0.8);
    const double drift_stddev = p.get<double>("drift-stddev", 1.0);
    const int max_drift    = p.get<int>("max-drift", 2);

    // simulation params
    const int num_segments = p.get<int>("segments" , 2);

    estd::Reseed(seed);

    // メルセンヌ・ツイスタの生成する値のうち、最初の100万個を捨てる (分布が良くないらしい)
    for (int i = 0; i < 1e+6; ++i) {
        estd::Random();
    }

    std::cout << "code parameters:\n"
              << "    code length: " << code_length << '\n'
              << "channel parameters:\n"
              << "    ps: " << ps << '\n'
              << "    pass ratio: " << pass_ratio << '\n'
              << "    drift stddev: " << drift_stddev << '\n'
              << "    max drift: " << max_drift << '\n'
              << "simulation parameters:\n"
              << "    num threads: " << num_threads << '\n'
              << "    num simulations: " << num_simulations << '\n'
              << "    num segments: " << num_segments << '\n'
              << std::endl;

    channel::TDCParams tdc_params;
    tdc_params.ps = ps;
    tdc_params.pass_ratio = pass_ratio;
    tdc_params.drift_stddev = drift_stddev;
    tdc_params.max_drift = max_drift;

    channel::TDC tdc(tdc_params);

    TDCCapacityCalculator::Params calculator_params;
    calculator_params.code_length = code_length;
    calculator_params.num_simulations = num_simulations;
    calculator_params.num_segments = num_segments;

    TDCCapacityCalculator calculator(calculator_params, tdc);
    const double c = calculator.parallel_calculate(num_threads);
    std::cout << c << std::endl;

    return 0;
}
