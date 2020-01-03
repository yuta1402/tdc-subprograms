#ifndef PCSTDC_REC_CALCULATION_ELEMENT_HPP
#define PCSTDC_REC_CALCULATION_ELEMENT_HPP

#include <array>

/*!
@namespace pcstdc
@brief Polar Code Subprograms for Timing Drift Channel
*/
namespace pcstdc
{
    /*!
    @class RecCalculationElement
    @brief the element of recursively calculations
    */
    struct RecCalculationElement
    {
        int prev_index{ -1 };
        std::array<long double, 2> value{ -1.0, -1.0 };
    };
}

#endif
