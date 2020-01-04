#ifndef PCSTDC_REC_CALCULATION_ELEMENT_HPP
#define PCSTDC_REC_CALCULATION_ELEMENT_HPP

#include <array>
#include "estd/negative_index_vector.hpp"

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
        estd::nivector<estd::nivector<std::array<long double, 2>>> value;
    };
}

#endif
