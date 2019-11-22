#include "test.hpp"
#include "bsc_test.hpp"
#include "tdc_test.hpp"

int main()
{
    bsc_test();
    tdc_test();

    return test::exit();
}
