#include "test.hpp"
#include "utl/utility.hpp"

void num_digits_test()
{
    eassert(utl::num_digits(0) == 1, "utl::num_digits(0) == %ld", utl::num_digits(0));
    eassert(utl::num_digits(1) == 1, "utl::num_digits(1) == %ld", utl::num_digits(1));
    eassert(utl::num_digits(9) == 1, "utl::num_digits(9) == %ld", utl::num_digits(9));
    eassert(utl::num_digits(10) == 2, "utl::num_digits(10) == %ld", utl::num_digits(10));
    eassert(utl::num_digits(99) == 2, "utl::num_digits(99) == %ld", utl::num_digits(99));
    eassert(utl::num_digits(100) == 3, "utl::num_digits(100) == %ld", utl::num_digits(100));
    eassert(utl::num_digits(999) == 3, "utl::num_digits(999) == %ld", utl::num_digits(999));
}

void num_decimal_places()
{
    // TODO: Fix
    // eassert(utl::num_decimal_places(0.0) == 0, "utl::num_decimal_places(0.0) == %ld", utl::num_decimal_places(0.0));
    // eassert(utl::num_decimal_places(0.1) == 1, "utl::num_decimal_places(0.1) == %ld", utl::num_decimal_places(0.1));
    // eassert(utl::num_decimal_places(0.9) == 1, "utl::num_decimal_places(0.9) == %ld", utl::num_decimal_places(0.9));
    // eassert(utl::num_decimal_places(9.9) == 1, "utl::num_decimal_places(9.9) == %ld", utl::num_decimal_places(9.9));
    // eassert(utl::num_decimal_places(0.01) == 2, "utl::num_decimal_places(0.01) == %ld", utl::num_decimal_places(0.01));
    // eassert(utl::num_decimal_places(0.09) == 2, "utl::num_decimal_places(0.09) == %ld", utl::num_decimal_places(0.09));
    // eassert(utl::num_decimal_places(0.99) == 2, "utl::num_decimal_places(0.99) == %ld", utl::num_decimal_places(0.99));
    // eassert(utl::num_decimal_places(9.99) == 2, "utl::num_decimal_places(9.99) == %ld", utl::num_decimal_places(9.99));
}

void utility_test()
{
    num_digits_test();
    num_decimal_places();
}
