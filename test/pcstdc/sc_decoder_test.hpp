#include "test.hpp"
#include "channel/tdc.hpp"
#include "pcstdc/sc_decoder.hpp"
#include "pcstdc/polar_encoder.hpp"

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

void sc_decoder_decode_test()
{
    const size_t code_length = 4;
    const size_t info_length = 2;
    const std::vector<int> frozen_bits{ 1, 1, 0, 0 };

    channel::TDCParams tdc_params;
    tdc_params.pass_ratio = 0.8;
    tdc_params.drift_stddev = 1.0;
    tdc_params.max_drift = 2;
    tdc_params.ps = 0.0;

    pcstdc::SCDecoderParams decoder_params;
    decoder_params.code_length = code_length;
    decoder_params.info_length = info_length;
    decoder_params.num_segments = 2;
    decoder_params.frozen_bits = frozen_bits;

    pcstdc::PolarEncoder encoder(code_length, info_length, frozen_bits);
    pcstdc::SCDecoder decoder(decoder_params, tdc_params);

    Eigen::RowVectorXi m(2);

    {
        m << 0, 0;
        auto x = encoder.encode(m);
        auto m_hat = decoder.decode(x);

        for (int i = 0; i < m.size(); ++i) {
            eassert(m_hat[i] == m[i], "m_hat[%d] == %d, m[%d] == %d", i, m_hat[i], i, m[i]);
        }
    }

    {
        m << 0, 1;
        auto x = encoder.encode(m);
        auto m_hat = decoder.decode(x);

        for (int i = 0; i < m.size(); ++i) {
            eassert(m_hat[i] == m[i], "m_hat[%d] == %d, m[%d] == %d", i, m_hat[i], i, m[i]);
        }
    }

    {
        m << 1, 0;
        auto x = encoder.encode(m);
        auto m_hat = decoder.decode(x);

        for (int i = 0; i < m.size(); ++i) {
            eassert(m_hat[i] == m[i], "m_hat[%d] == %d, m[%d] == %d", i, m_hat[i], i, m[i]);
        }
    }

    {
        m << 1, 1;
        auto x = encoder.encode(m);
        auto m_hat = decoder.decode(x);

        for (int i = 0; i < m.size(); ++i) {
            eassert(m_hat[i] == m[i], "m_hat[%d] == %d, m[%d] == %d", i, m_hat[i], i, m[i]);
        }
    }
}

void sc_decoder_test()
{
    sc_decoder_calc_level0_test();
    sc_decoder_calc_level1_test();
    sc_decoder_decode_test();
}
