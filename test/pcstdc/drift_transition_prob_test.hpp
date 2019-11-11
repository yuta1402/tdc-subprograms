#include "test.hpp"
#include "pcstdc/drift_transition_prob.hpp"

void drift_transition_prob_test()
{
    pcstdc::DriftTransitionProb dtp(0.0, 1.0, 2, 2);
    std::cout << dtp(0, -2) << std::endl;
    std::cout << dtp(0, -1) << std::endl;
    std::cout << dtp(0,  0) << std::endl;
    std::cout << dtp(0,  1) << std::endl;
    std::cout << dtp(0,  2) << std::endl;
}
