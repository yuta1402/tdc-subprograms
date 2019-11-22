#include "test.hpp"
#include "channel/bsc.hpp"

void bsc_send_test()
{
    {
        channel::BSC bsc(0.0);

        for (int i = 0; i < 10; ++i) {
            int y = bsc.send(0);
            eassert(y == 0, "y == %d", y);
        }
    }

    {
        channel::BSC bsc(1.0);

        for (int i = 0; i < 10; ++i) {
            int y = bsc.send(0);
            eassert(y == 1, "y == %d", y);
        }
    }
}

void bsc_send_vector_test()
{
    {
        channel::BSC bsc(0.0);
        auto x = Eigen::RowVectorXi::Zero(10);
        auto y = bsc.send(x);

        for (int i = 0; i < y.size(); ++i) {
            eassert(y[i] == 0, "y[i] == %d", y[i]);
        }
    }

    {
        channel::BSC bsc(1.0);
        auto x = Eigen::RowVectorXi::Zero(10);
        auto y = bsc.send(x);

        for (int i = 0; i < y.size(); ++i) {
            eassert(y[i] == 1, "y[i] == %d", y[i]);
        }
    }
}

void bsc_prob_test()
{
    channel::BSC bsc(0.1);

    eassert(bsc.prob(0, 0) == 0.9, "bsc.prob(0, 0) == %f", bsc.prob(0, 0));
    eassert(bsc.prob(0, 1) == 0.1, "bsc.prob(0, 1) == %f", bsc.prob(0, 1));
    eassert(bsc.prob(1, 0) == 0.1, "bsc.prob(1, 0) == %f", bsc.prob(1, 0));
    eassert(bsc.prob(1, 1) == 0.9, "bsc.prob(1, 1) == %f", bsc.prob(1, 1));
}

void bsc_calc_capacity_test()
{
    {
        channel::BSC bsc(0.0);
        double c = bsc.calc_capacity();
        eassert(c == 1.0, "c == %f", c);
    }

    {
        channel::BSC bsc(0.5);
        double c = bsc.calc_capacity();
        eassert(c == 0.0, "c == %f", c);
    }

    {
        channel::BSC bsc(1.0);
        double c = bsc.calc_capacity();
        eassert(c == 1.0, "c == %f", c);
    }
}

void bsc_test()
{
    bsc_send_test();
    bsc_send_vector_test();
    bsc_prob_test();
    bsc_calc_capacity_test();
}
