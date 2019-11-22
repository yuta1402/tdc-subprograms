#include "test.hpp"
#include "drift_transition_prob_test.hpp"
#include "sc_decoder_test.hpp"
#include "polar_code_encoder_test.hpp"

int main()
{
    drift_transition_prob_test();
    sc_decoder_test();
    polar_code_encoder_test();

    return test::exit();
}
