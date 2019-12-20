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

void cyclic_prior_prob_marker01_test()
{
    {
        auto p = utl::CyclicPriorProb::Marker01(0);
        eassert(p[0][0] == 1.0, "p[0][0] == %lf", p[0][0]);
        eassert(p[1][0] == 0.0, "p[1][0] == %lf", p[1][0]);
        eassert(p[2][0] == 0.0, "p[2][0] == %lf", p[2][0]);
        eassert(p[3][0] == 1.0, "p[3][0] == %lf", p[3][0]);

        eassert(p[0][1] == 0.0, "p[0][1] == %lf", p[0][1]);
        eassert(p[1][1] == 1.0, "p[1][1] == %lf", p[1][1]);
        eassert(p[2][1] == 1.0, "p[2][1] == %lf", p[2][1]);
        eassert(p[3][1] == 0.0, "p[3][1] == %lf", p[3][1]);

        eassert(p[4][0] == 1.0, "p[4][0] == %lf", p[4][0]);
        eassert(p[5][0] == 0.0, "p[5][0] == %lf", p[5][0]);
        eassert(p[6][0] == 0.0, "p[6][0] == %lf", p[6][0]);
        eassert(p[7][0] == 1.0, "p[7][0] == %lf", p[7][0]);

        eassert(p[4][1] == 0.0, "p[4][1] == %lf", p[4][1]);
        eassert(p[5][1] == 1.0, "p[5][1] == %lf", p[5][1]);
        eassert(p[6][1] == 1.0, "p[6][1] == %lf", p[6][1]);
        eassert(p[7][1] == 0.0, "p[7][1] == %lf", p[7][1]);
    }

    {
        auto p = utl::CyclicPriorProb::Marker01(1);
        eassert(p[0][0] == 0.5, "p[0][0] == %lf", p[0][0]);
        eassert(p[1][0] == 1.0, "p[1][0] == %lf", p[1][0]);
        eassert(p[2][0] == 0.0, "p[2][0] == %lf", p[2][0]);
        eassert(p[3][0] == 0.5, "p[3][0] == %lf", p[3][0]);
        eassert(p[4][0] == 0.0, "p[4][0] == %lf", p[4][0]);
        eassert(p[5][0] == 1.0, "p[5][0] == %lf", p[5][0]);

        eassert(p[0][1] == 0.5, "p[0][1] == %lf", p[0][1]);
        eassert(p[1][1] == 0.0, "p[1][1] == %lf", p[1][1]);
        eassert(p[2][1] == 1.0, "p[2][1] == %lf", p[2][1]);
        eassert(p[3][1] == 0.5, "p[3][1] == %lf", p[3][1]);
        eassert(p[4][1] == 1.0, "p[4][1] == %lf", p[4][1]);
        eassert(p[5][1] == 0.0, "p[5][1] == %lf", p[5][1]);

        eassert(p[6][0] == 0.5 , "p[6][0] == %lf" , p[6][0]);
        eassert(p[7][0] == 1.0 , "p[7][0] == %lf" , p[7][0]);
        eassert(p[8][0] == 0.0 , "p[8][0] == %lf" , p[8][0]);
        eassert(p[9][0] == 0.5 , "p[9][0] == %lf" , p[9][0]);
        eassert(p[10][0] == 0.0, "p[10][0] == %lf", p[10][0]);
        eassert(p[11][0] == 1.0, "p[11][0] == %lf", p[11][0]);

        eassert(p[6][1] == 0.5 , "p[6][1] == %lf" , p[6][1]);
        eassert(p[7][1] == 0.0 , "p[7][1] == %lf" , p[7][1]);
        eassert(p[8][1] == 1.0 , "p[8][1] == %lf" , p[8][1]);
        eassert(p[9][1] == 0.5 , "p[9][1] == %lf" , p[9][1]);
        eassert(p[10][1] == 1.0, "p[10][1] == %lf", p[10][1]);
        eassert(p[11][1] == 0.0, "p[11][1] == %lf", p[11][1]);
    }

    {
        auto p = utl::CyclicPriorProb::Marker01(2);
        eassert(p[0][0] == 0.5, "p[0][0] == %lf", p[0][0]);
        eassert(p[1][0] == 0.5, "p[1][0] == %lf", p[1][0]);
        eassert(p[2][0] == 1.0, "p[2][0] == %lf", p[2][0]);
        eassert(p[3][0] == 0.0, "p[3][0] == %lf", p[3][0]);
        eassert(p[4][0] == 0.5, "p[4][0] == %lf", p[4][0]);
        eassert(p[5][0] == 0.5, "p[5][0] == %lf", p[5][0]);
        eassert(p[6][0] == 0.0, "p[6][0] == %lf", p[6][0]);
        eassert(p[7][0] == 1.0, "p[7][0] == %lf", p[7][0]);

        eassert(p[0][1] == 0.5, "p[0][1] == %lf", p[0][1]);
        eassert(p[1][1] == 0.5, "p[1][1] == %lf", p[1][1]);
        eassert(p[2][1] == 0.0, "p[2][1] == %lf", p[2][1]);
        eassert(p[3][1] == 1.0, "p[3][1] == %lf", p[3][1]);
        eassert(p[4][1] == 0.5, "p[4][1] == %lf", p[4][1]);
        eassert(p[5][1] == 0.5, "p[5][1] == %lf", p[5][1]);
        eassert(p[6][1] == 1.0, "p[6][1] == %lf", p[6][1]);
        eassert(p[7][1] == 0.0, "p[7][1] == %lf", p[7][1]);

        eassert(p[8][0] == 0.5 , "p[8][0] == %lf" , p[8][0]);
        eassert(p[9][0] == 0.5 , "p[9][0] == %lf" , p[9][0]);
        eassert(p[10][0] == 1.0, "p[10][0] == %lf", p[10][0]);
        eassert(p[11][0] == 0.0, "p[11][0] == %lf", p[11][0]);
        eassert(p[12][0] == 0.5, "p[12][0] == %lf", p[12][0]);
        eassert(p[13][0] == 0.5, "p[13][0] == %lf", p[13][0]);
        eassert(p[14][0] == 0.0, "p[14][0] == %lf", p[14][0]);
        eassert(p[15][0] == 1.0, "p[15][0] == %lf", p[15][0]);

        eassert(p[8][1] == 0.5 , "p[8][1] == %lf" , p[8][1]);
        eassert(p[9][1] == 0.5 , "p[9][1] == %lf" , p[9][1]);
        eassert(p[10][1] == 0.0, "p[10][1] == %lf", p[10][1]);
        eassert(p[11][1] == 1.0, "p[11][1] == %lf", p[11][1]);
        eassert(p[12][1] == 0.5, "p[12][1] == %lf", p[12][1]);
        eassert(p[13][1] == 0.5, "p[13][1] == %lf", p[13][1]);
        eassert(p[14][1] == 1.0, "p[14][1] == %lf", p[14][1]);
        eassert(p[15][1] == 0.0, "p[15][1] == %lf", p[15][1]);
    }
}

void cyclic_prior_prob_test()
{
    cyclic_prior_prob_constructor_test();
    cyclic_prior_prob_marker01_test();
}
