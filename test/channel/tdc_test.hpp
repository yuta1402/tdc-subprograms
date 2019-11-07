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
            double nd = tdc.generate_next_drift_value(0.0); eassert(-1.0 <= nd && nd <= 1.0, "nd == %lf", nd);
        }
    }

    {
        params.pass_ratio = 0.5;

        channel::TDC tdc(params);

        for (int i = 0; i < 100; ++i) {
            double nd = tdc.generate_next_drift_value(0.0); eassert(-0.5 <= nd && nd <= 0.5, "nd == %lf", nd);
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

    // start from di=1.0
    {
        params.pass_ratio = 0.0;

        channel::TDC tdc(params);

        for (int i = 0; i < 100; ++i) {
            double nd = tdc.generate_next_drift_value(1.0); eassert(0.0 <= nd && nd <= 2.0, "nd == %lf", nd);
        }
    }

    // start from di=-1.0
    {
        params.pass_ratio = 0.0;

        channel::TDC tdc(params);

        for (int i = 0; i < 100; ++i) {
            double nd = tdc.generate_next_drift_value(-1.0); eassert(-2.0 <= nd && nd <= 0.0, "nd == %lf", nd);
        }
    }

    // start from positive bounrdary (D=4.0)
    {
        params.pass_ratio = 0.0;

        channel::TDC tdc(params);

        for (int i = 0; i < 100; ++i) {
            double nd = tdc.generate_next_drift_value(4.0); eassert(3.0 <= nd && nd <= 4.0, "nd == %lf", nd);
        }
    }

    // start from negative bounrdary (D=-4.0)
    {
        params.pass_ratio = 0.0;

        channel::TDC tdc(params);

        for (int i = 0; i < 100; ++i) {
            double nd = tdc.generate_next_drift_value(-4.0); eassert(-4.0 <= nd && nd <= -3.0, "nd == %lf", nd);
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
