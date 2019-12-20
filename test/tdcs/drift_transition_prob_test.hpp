#include "test.hpp"
#include "tdcs/drift_transition_prob.hpp"

void drift_transition_prob_test()
{
    tdcs::DriftTransitionProb dtp(0.0, 1.0, 2, 2);

    eassert(dtp(-4, -1) == 0.0000000000e+00, "dtp(-4, -1) == %lf", dtp(-4, -1));
    eassert(dtp(-3, -1) == 9.9565174546e-02, "dtp(-3, -1) == %lf", dtp(-3, -1));
    eassert(dtp(-2, -1) == 2.5585031548e-01, "dtp(-2, -1) == %lf", dtp(-2, -1));
    eassert(dtp(-1, -1) == 2.8916901994e-01, "dtp(-1, -1) == %lf", dtp(-1, -1));
    eassert(dtp( 0, -1) == 2.5585031548e-01, "dtp( 0, -1) == %lf", dtp( 0, -1));
    eassert(dtp( 1, -1) == 9.9565174546e-02, "dtp( 1, -1) == %lf", dtp( 1, -1));
    eassert(dtp( 2, -1) == 0.00000000000+00, "dtp( 2, -1) == %lf", dtp( 2, -1));
    eassert(dtp( 3, -1) == 0.00000000000+00, "dtp( 3, -1) == %lf", dtp( 3, -1));
    eassert(dtp( 4, -1) == 0.00000000000+00, "dtp( 4, -1) == %lf", dtp( 4, -1));

    eassert(dtp(-4, 0) == 0.0000000000e+00, "dtp(-4, 0) == %lf", dtp(-4, 0));
    eassert(dtp(-3, 0) == 0.0000000000e+00, "dtp(-3, 0) == %lf", dtp(-3, 0));
    eassert(dtp(-2, 0) == 9.9565174546e-02, "dtp(-2, 0) == %lf", dtp(-2, 0));
    eassert(dtp(-1, 0) == 2.5585031548e-01, "dtp(-1, 0) == %lf", dtp(-1, 0));
    eassert(dtp( 0, 0) == 2.8916901994e-01, "dtp( 0, 0) == %lf", dtp( 0, 0));
    eassert(dtp( 1, 0) == 2.5585031548e-01, "dtp( 1, 0) == %lf", dtp( 1, 0));
    eassert(dtp( 2, 0) == 9.9565174546e-02, "dtp( 2, 0) == %lf", dtp( 2, 0));
    eassert(dtp( 3, 0) == 0.00000000000+00, "dtp( 3, 0) == %lf", dtp( 3, 0));
    eassert(dtp( 4, 0) == 0.00000000000+00, "dtp( 4, 0) == %lf", dtp( 4, 0));

    eassert(dtp(-4, 1) == 0.0000000000e+00, "dtp(-4, 1) == %lf", dtp(-4, 1));
    eassert(dtp(-3, 1) == 0.0000000000e+00, "dtp(-3, 1) == %lf", dtp(-3, 1));
    eassert(dtp(-2, 1) == 0.0000000000e+00, "dtp(-2, 1) == %lf", dtp(-2, 1));
    eassert(dtp(-1, 1) == 9.9565174546e-02, "dtp(-1, 1) == %lf", dtp(-1, 1));
    eassert(dtp( 0, 1) == 2.5585031548e-01, "dtp( 0, 1) == %lf", dtp( 0, 1));
    eassert(dtp( 1, 1) == 2.8916901994e-01, "dtp( 1, 1) == %lf", dtp( 1, 1));
    eassert(dtp( 2, 1) == 2.5585031548e-01, "dtp( 2, 1) == %lf", dtp( 2, 1));
    eassert(dtp( 3, 1) == 9.9565174546e-02, "dtp( 3, 1) == %lf", dtp( 3, 1));
    eassert(dtp( 4, 1) == 0.00000000000+00, "dtp( 4, 1) == %lf", dtp( 4, 1));
}
