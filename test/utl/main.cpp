#include "test.hpp"
#include "utility_test.hpp"
#include "ber_simulator_test.hpp"
#include "crc_handler_test.hpp"
#include "cyclic_prior_prob_test.hpp"
#include "raw_encoder_test.hpp"
#include "raw_decoder_test.hpp"

int main()
{
    utility_test();
    ber_simulator_test();
    crc_handler_test();
    cyclic_prior_prob_test();
    raw_encoder_test();
    raw_decoder_test();

    return test::exit();
}
