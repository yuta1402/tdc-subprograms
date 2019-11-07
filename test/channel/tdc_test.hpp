#include "estd/random.hpp"
#include "test.hpp"
#include "channel/tdc.hpp"

void tdc_generate_next_drift_value_test()
{
    channel::TDCParams params;

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
    channel::TDCParams params;

    params.drift_stddev = 5.0;
    params.max_drift = 4;

    {
        params.pass_ratio = 1.0;
        channel::TDC tdc(params);

        auto d = tdc.generate_drift_sequence(100);
        for (int i = 0; i < d.size(); ++i) {
            eassert(d[i] == 0.0, "%lf", d[i]);
        }
    }

    {
        params.pass_ratio = 0.0;
        channel::TDC tdc(params);

        auto d = tdc.generate_drift_sequence(100);
        for (int i = 0; i < d.size(); ++i) {
            eassert(-params.max_drift <= d[i] && d[i] <= params.max_drift, "d[%d] == %lf, max_drift == %d", i, d[i], params.max_drift);
        }
    }
}

void tdc_generate_signal_sequence_test()
{
    {
        channel::TDCParams params;

        params.pass_ratio = 0.8;
        params.drift_stddev = 5.0;
        params.max_drift = 4;

        channel::TDC tdc(params);

        Eigen::RowVectorXi x(11);
        x << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

        Eigen::RowVectorXd d(11);
        d << -0.5, -0.4, -0.3, -0.2, -0.1, 0.0, -0.1, 0.2, -0.3, 0.4, 0.5;

        auto y = tdc.generate_signal_sequence(x, d);

        eassert(y[ 0] == 2, "y[ 0] == %d", y[ 0]);
        eassert(y[ 1] == 2, "y[ 1] == %d", y[ 1]);
        eassert(y[ 2] == 0, "y[ 2] == %d", y[ 2]);
        eassert(y[ 3] == 0, "y[ 3] == %d", y[ 3]);
        eassert(y[ 4] == 0, "y[ 4] == %d", y[ 4]);
        eassert(y[ 5] == 0, "y[ 5] == %d", y[ 5]);
        eassert(y[ 6] == 0, "y[ 6] == %d", y[ 6]);
        eassert(y[ 7] == 0, "y[ 7] == %d", y[ 7]);
        eassert(y[ 8] == 0, "y[ 8] == %d", y[ 8]);
        eassert(y[ 9] == 2, "y[ 9] == %d", y[ 9]);
        eassert(y[10] == 2, "y[10] == %d", y[10]);
    }

    {
        channel::TDCParams params;

        params.pass_ratio = 0.8;
        params.drift_stddev = 5.0;
        params.max_drift = 4;

        channel::TDC tdc(params);

        Eigen::RowVectorXi x(11);
        x << 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0;

        Eigen::RowVectorXd d(11);
        d << 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0;

        auto y = tdc.generate_signal_sequence(x, d);

        eassert(y[ 0] == 2, "y[ 0] == %d", y[ 0]);
        eassert(y[ 1] == 0, "y[ 1] == %d", y[ 1]);
        eassert(y[ 2] == 1, "y[ 2] == %d", y[ 2]);
        eassert(y[ 3] == 0, "y[ 3] == %d", y[ 3]);
        eassert(y[ 4] == 1, "y[ 4] == %d", y[ 4]);
        eassert(y[ 5] == 2, "y[ 5] == %d", y[ 5]);
        eassert(y[ 6] == 2, "y[ 6] == %d", y[ 6]);
        eassert(y[ 7] == 2, "y[ 7] == %d", y[ 7]);
        eassert(y[ 8] == 2, "y[ 8] == %d", y[ 8]);
        eassert(y[ 9] == 1, "y[ 9] == %d", y[ 9]);
        eassert(y[10] == 0, "y[10] == %d", y[10]);
    }
}

void tdc_generate_received_word_test()
{
    {
        channel::TDCParams params;
        params.ps = 0.0;

        Eigen::RowVectorXi y(10);
        y << 2, 2, 0, 0, 0, 0, 0, 0, 2, 2;

        channel::TDC tdc(params);
        auto z = tdc.generate_received_word(y);

        for (int i = 2; i <= 7; ++i) {
            eassert(z[i] == 0, "z[%d] == %d", i, z[i]);
        }
    }

    {
        channel::TDCParams params;
        params.ps = 1.0;

        Eigen::RowVectorXi y(10);
        y << 2, 2, 0, 0, 0, 0, 0, 0, 2, 2;

        channel::TDC tdc(params);
        auto z = tdc.generate_received_word(y);

        for (int i = 2; i <= 7; ++i) {
            eassert(z[i] == 1, "z[%d] == %d", i, z[i]);
        }
    }
}

void tdc_send_test()
{
    channel::TDCParams params;

    params.drift_stddev = 5.0;
    params.max_drift = 4;
    params.pass_ratio = 0.8;
    params.ps = 0.1;

    channel::TDC tdc(params);

    Eigen::RowVectorXi x(100);
    for (int i = 0; i < x.size(); ++i) {
        x[i] = estd::Random(0, 1);
    }

    auto z = tdc.send(x);

    for (int i = 0; i < z.size(); ++i) {
        eassert(z[i] == 0 || z[i] == 1, "z[%d] == %d", i, z[i]);
    }

    eassert(z.size() == x.size(), "z.size() == %ld, x.size() == %ld", z.size(), x.size());
}

void tdc_test()
{
    tdc_generate_next_drift_value_test();
    tdc_generate_drift_sequence_test();
    tdc_generate_signal_sequence_test();
    tdc_generate_received_word_test();
    tdc_send_test();
}
