#include "test.hpp"
#include "utl/cyclic_prior_prob.hpp"

void cyclic_prior_prob_constructor_test()
{
    {
        utl::CyclicPriorProb p(2);

        eassert(p[0][0] == 0.5, "p[0][0] == %lf", p[0][0]);
        eassert(p[1][0] == 0.5, "p[1][0] == %lf", p[1][0]);
        eassert(p[2][0] == 0.5, "p[2][0] == %lf", p[2][0]);
        eassert(p[3][0] == 0.5, "p[3][0] == %lf", p[3][0]);

        eassert(p[0][1] == 0.5, "p[0][1] == %lf", p[0][1]);
        eassert(p[1][1] == 0.5, "p[1][1] == %lf", p[1][1]);
        eassert(p[2][1] == 0.5, "p[2][1] == %lf", p[2][1]);
        eassert(p[3][1] == 0.5, "p[3][1] == %lf", p[3][1]);
    }

    {
        utl::CyclicPriorProb p(2, 0.6);

        eassert(p[0][0] == 0.6, "p[0][0] == %lf", p[0][0]);
        eassert(p[1][0] == 0.6, "p[1][0] == %lf", p[1][0]);
        eassert(p[2][0] == 0.6, "p[2][0] == %lf", p[2][0]);
        eassert(p[3][0] == 0.6, "p[3][0] == %lf", p[3][0]);

        eassert(p[0][1] == 0.4, "p[0][1] == %lf", p[0][1]);
        eassert(p[1][1] == 0.4, "p[1][1] == %lf", p[1][1]);
        eassert(p[2][1] == 0.4, "p[2][1] == %lf", p[2][1]);
        eassert(p[3][1] == 0.4, "p[3][1] == %lf", p[3][1]);
    }
}

void cyclic_prior_prob_test()
{
    cyclic_prior_prob_constructor_test();
}
