#include "test.hpp"
#include "sc_decoder_test.hpp"
#include "scl_decoder_test.hpp"
#include "scl_crc_decoder_test.hpp"
#include "polar_encoder_test.hpp"

int main()
{
    sc_decoder_test();
    scl_decoder_test();
    scl_crc_decoder_test();
    polar_encoder_test();

    return test::exit();
}
