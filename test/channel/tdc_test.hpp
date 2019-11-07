#include "test.hpp"
#include "channel/tdc.hpp"

void tdc_generate_next_drift_value_test()
{
    channel::TDCParams params;

    params.ps = 0.1;
    params.drift_stddev = 5.0;

    params.max_drift = 4;

    {
        params.pass_ratio = 0.0;

        channel::TDC tdc(params);

        for (int i = 0; i < 100; ++i) {
            double nd = tdc.generate_next_drift_value(0.0); eassert(abs(nd) <= 1.0-params.pass_ratio, "abs(nd) == %lf, 1.0-pass_ratio == %lf", abs(nd), 1.0-params.pass_ratio);
        }
    }

    {
        params.pass_ratio = 0.5;

        channel::TDC tdc(params);

        for (int i = 0; i < 100; ++i) {
            double nd = tdc.generate_next_drift_value(0.0); eassert(abs(nd) <= 1.0-params.pass_ratio, "abs(nd) == %lf, 1.0-pass_ratio == %lf", abs(nd), 1.0-params.pass_ratio);
        }
    }

    {
        params.pass_ratio = 1.0;

        channel::TDC tdc(params);

        for (int i = 0; i < 100; ++i) {
            double nd = tdc.generate_next_drift_value(0.0); eassert(nd == 0.0, "nd == %lf", nd);
        }
    }

    {
        params.pass_ratio = 2.0;

        channel::TDC tdc(params);

        for (int i = 0; i < 100; ++i) {
            double nd = tdc.generate_next_drift_value(0.0); eassert(nd == 0.0, "nd == %lf", nd);
        }
    }
}

void tdc_generate_drift_sequence_test()
{
}

void tdc_generate_signal_sequence_test()
{
}

void tdc_generate_received_word_test()
{
}

void tdc_test()
{
    tdc_generate_next_drift_value_test();
    tdc_generate_drift_sequence_test();
    tdc_generate_signal_sequence_test();
    tdc_generate_received_word_test();
}
