
#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstddef>
#include <cmath>

/*!
@namespace utl
@brief utility
*/
namespace utl
{
    template<class Type>
    size_t num_digits(Type x)
    {
        if (x == 0) {
            return 1;
        }

        if (x < 0) {
            x = -x;
        }

        return std::floor(std::log10(x)) + 1;
    }

    // TODO: Fix
    // template<class Type>
    // size_t num_decimal_places(Type x)
    // {
    //     x -= (int)x;
    //
    //     if (x == 0) {
    //         return 0;
    //     }
    //
    //     return std::ceil(-std::log10(std::abs(x)));
    // }
}

#endif
