#include "test.hpp"
#include "drift_transition_prob_test.hpp"
#include "likelihood_calculator_test.hpp"

int main()
{
    drift_transition_prob_test();
    likelihood_calculator_test();

    return test::exit();
}
