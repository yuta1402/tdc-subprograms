#include "test.hpp"
#include "channel/tdc.hpp"
#include "pcstdc/sc_decoder.hpp"

void sc_decoder_calc_level0_test()
{
    const double ps = 0.1;

    channel::TDCParams tdc_params;
    tdc_params.pass_ratio = 0.8;
    tdc_params.drift_stddev = 1.0;
    tdc_params.max_drift = 2;
    tdc_params.ps = ps;

    pcstdc::SCDecoderParams decoder_params;
    decoder_params.code_length = 8;
    decoder_params.info_length = 4;
    decoder_params.num_segments = 2;

    pcstdc::SCDecoder decoder(decoder_params, tdc_params);

    Eigen::RowVectorXi z(8);
    z << 0, 1, 0, 1, 0, 1, 0, 1;

    {
        double p;
        p = decoder.calc_level0(0, -2, 0, z); eassert(p == 0.5, "p == %lf", p);
        p = decoder.calc_level0(0, -1, 0, z); eassert(p == 0.5, "p == %lf", p);
        p = decoder.calc_level0(0,  0, 0, z); eassert(p == 1.0-ps, "p == %lf, ps == %lf", p, ps);
        p = decoder.calc_level0(0,  1, 0, z); eassert(p == 0.5, "p == %lf", p);
        p = decoder.calc_level0(0,  2, 0, z); eassert(p == ps, "p == %lf, ps == %lf", p, ps);
    }

    {
        double p;
        p = decoder.calc_level0(0, -2, 1, z); eassert(p == 0.5, "p == %lf", p);
        p = decoder.calc_level0(0, -1, 1, z); eassert(p == 0.5, "p == %lf", p);
        p = decoder.calc_level0(0,  0, 1, z); eassert(p == ps, "p == %lf, ps == %lf", p, ps);
        p = decoder.calc_level0(0,  1, 1, z); eassert(p == 0.5, "p == %lf", p);
        p = decoder.calc_level0(0,  2, 1, z); eassert(p == 1.0-ps, "p == %lf, ps == %lf", p, ps);
    }

    {
        double p;
        p = decoder.calc_level0(1, -2, 0, z); eassert(p == 1.0-ps, "p == %lf, ps == %lf", p, ps);
        p = decoder.calc_level0(1, -1, 0, z); eassert(p == 0.5, "p == %lf", p);
        p = decoder.calc_level0(1,  0, 0, z); eassert(p == ps, "p == %lf, ps == %lf", p, ps);
        p = decoder.calc_level0(1,  1, 0, z); eassert(p == 0.5, "p == %lf", p);
        p = decoder.calc_level0(1,  2, 0, z); eassert(p == 1.0-ps, "p == %lf, ps == %lf", p, ps);
    }

    {
        double p;
        p = decoder.calc_level0(1, -2, 1, z); eassert(p == ps, "p == %lf, ps == %lf", p, ps);
        p = decoder.calc_level0(1, -1, 1, z); eassert(p == 0.5, "p == %lf", p);
        p = decoder.calc_level0(1,  0, 1, z); eassert(p == 1.0-ps, "p == %lf, ps == %lf", p, ps);
        p = decoder.calc_level0(1,  1, 1, z); eassert(p == 0.5, "p == %lf", p);
        p = decoder.calc_level0(1,  2, 1, z); eassert(p == ps, "p == %lf, ps == %lf", p, ps);
    }
}

void sc_decoder_calc_level1_test()
{
    // TODO: Implement
}

void sc_decoder_test()
{
    sc_decoder_calc_level0_test();
    sc_decoder_calc_level1_test();
}
