#include "test.hpp"
#include "utl/cyclic_prior_prob.hpp"

void cyclic_prior_prob_test()
{
    {
        auto p = utl::CyclicPriorProb::Marker01(0);
        eassert(p[0] == 1.0, "p[0] == %lf", p[0]);
        eassert(p[1] == 0.0, "p[1] == %lf", p[1]);
        eassert(p[2] == 0.0, "p[2] == %lf", p[2]);
        eassert(p[3] == 1.0, "p[3] == %lf", p[3]);

        eassert(p[4] == 1.0, "p[4] == %lf", p[4]);
        eassert(p[5] == 0.0, "p[5] == %lf", p[5]);
        eassert(p[6] == 0.0, "p[6] == %lf", p[6]);
        eassert(p[7] == 1.0, "p[7] == %lf", p[7]);
    }

    {
        auto p = utl::CyclicPriorProb::Marker01(1);
        eassert(p[0] == 0.5, "p[0] == %lf", p[0]);
        eassert(p[1] == 1.0, "p[1] == %lf", p[1]);
        eassert(p[2] == 0.0, "p[2] == %lf", p[2]);
        eassert(p[3] == 0.5, "p[3] == %lf", p[3]);
        eassert(p[4] == 0.0, "p[4] == %lf", p[4]);
        eassert(p[5] == 1.0, "p[5] == %lf", p[5]);

        eassert(p[6] == 0.5 , "p[6] == %lf" , p[6]);
        eassert(p[7] == 1.0 , "p[7] == %lf" , p[7]);
        eassert(p[8] == 0.0 , "p[8] == %lf" , p[8]);
        eassert(p[9] == 0.5 , "p[9] == %lf" , p[9]);
        eassert(p[10] == 0.0, "p[10] == %lf", p[10]);
        eassert(p[11] == 1.0, "p[11] == %lf", p[11]);
    }

    {
        auto p = utl::CyclicPriorProb::Marker01(2);
        eassert(p[0] == 0.5, "p[0] == %lf", p[0]);
        eassert(p[1] == 0.5, "p[1] == %lf", p[1]);
        eassert(p[2] == 1.0, "p[2] == %lf", p[2]);
        eassert(p[3] == 0.0, "p[3] == %lf", p[3]);
        eassert(p[4] == 0.5, "p[4] == %lf", p[4]);
        eassert(p[5] == 0.5, "p[5] == %lf", p[5]);
        eassert(p[6] == 0.0, "p[6] == %lf", p[6]);
        eassert(p[7] == 1.0, "p[7] == %lf", p[7]);

        eassert(p[8] == 0.5 , "p[8] == %lf" , p[8]);
        eassert(p[9] == 0.5 , "p[9] == %lf" , p[9]);
        eassert(p[10] == 1.0, "p[10] == %lf", p[10]);
        eassert(p[11] == 0.0, "p[11] == %lf", p[11]);
        eassert(p[12] == 0.5, "p[12] == %lf", p[12]);
        eassert(p[13] == 0.5, "p[13] == %lf", p[13]);
        eassert(p[14] == 0.0, "p[14] == %lf", p[14]);
        eassert(p[15] == 1.0, "p[15] == %lf", p[15]);
    }
}
