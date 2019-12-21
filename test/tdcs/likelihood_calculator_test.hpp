#include "test.hpp"
#include "tdcs/likelihood_calculator.hpp"
#include "channel/tdc.hpp"

void likelihood_calculator_test()
{
    channel::TDCParams tdc_params;
    tdc_params.ps = 0.0;
    tdc_params.drift_stddev = 1;
    tdc_params.max_drift = 2;
    tdc_params.pass_ratio = 0.8;

    channel::TDC tdc(tdc_params);

    tdcs::LikelihoodCalculator::Params params;
    params.num_segments = 2;
    params.prior_prob = utl::CyclicPriorProb::Marker01(2);

    tdcs::LikelihoodCalculator llc(tdc, params);

    Eigen::RowVectorXi z(8);
    z << 0, 0, 0, 1, 0, 0, 1, 0;

    auto ll = llc.calc(z);

    eassert(ll[0][0] == 1.0, "ll[0][0] == %lf", ll[0][0]);
    eassert(ll[1][0] == 1.0, "ll[1][0] == %lf", ll[1][0]);
    eassert(ll[2][0] == 1.0, "ll[2][0] == %lf", ll[2][0]);
    eassert(ll[3][0] == 0.0, "ll[3][0] == %lf", ll[3][0]);
    eassert(ll[4][0] == 1.0, "ll[4][0] == %lf", ll[4][0]);
    eassert(ll[5][0] == 1.0, "ll[5][0] == %lf", ll[5][0]);
    eassert(ll[6][0] == 0.0, "ll[6][0] == %lf", ll[6][0]);
    eassert(ll[7][0] == 1.0, "ll[7][0] == %lf", ll[7][0]);

    eassert(ll[0][1] == 0.0, "ll[0][1] == %lf", ll[0][1]);
    eassert(ll[1][1] == 0.0, "ll[1][1] == %lf", ll[1][1]);
    eassert(ll[2][1] == 0.0, "ll[2][1] == %lf", ll[2][1]);
    eassert(ll[3][1] == 1.0, "ll[3][1] == %lf", ll[3][1]);
    eassert(ll[4][1] == 0.0, "ll[4][1] == %lf", ll[4][1]);
    eassert(ll[5][1] == 0.0, "ll[5][1] == %lf", ll[5][1]);
    eassert(ll[6][1] == 1.0, "ll[6][1] == %lf", ll[6][1]);
    eassert(ll[7][1] == 0.0, "ll[7][1] == %lf", ll[7][1]);
}
