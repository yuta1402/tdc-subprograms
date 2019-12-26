#include "test.hpp"
#include "channel/tdc.hpp"
#include "pcstdc/scl_decoder.hpp"
#include "pcstdc/polar_encoder.hpp"

void scl_decoder_decode_test()
{
    const size_t code_length = 4;
    const size_t info_length = 2;
    const std::vector<int> frozen_bits{ 1, 1, 0, 0 };

    channel::TDCParams tdc_params;
    tdc_params.pass_ratio = 0.8;
    tdc_params.drift_stddev = 1.0;
    tdc_params.max_drift = 2;
    tdc_params.ps = 0.0;

    channel::TDC tdc(tdc_params);

    pcstdc::SCDecoderParams decoder_params;
    decoder_params.code_length = code_length;
    decoder_params.info_length = info_length;
    decoder_params.num_segments = 2;

    pcstdc::PolarEncoder encoder(code_length, info_length, frozen_bits);
    pcstdc::SCLDecoder decoder(decoder_params, tdc, frozen_bits, 4);

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

void scl_decoder_test()
{
    scl_decoder_decode_test();
}
