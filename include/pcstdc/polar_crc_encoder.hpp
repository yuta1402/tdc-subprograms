#ifndef PCSTDC_POLAR_CODE_CRC_ENCODER_HPP
#define PCSTDC_POLAR_CODE_CRC_ENCODER_HPP

#include <vector>
#include <Eigen/Core>
#include "pcstdc/polar_encoder.hpp"
#include "utl/crc_handler.hpp"

/*!
@namespace pcstdc
@brief Polar Code Subprograms for Timing Drift Channel
*/
namespace pcstdc
{
    /*!
    @class PolarCRCEncoder
    @brief Polar Code Encoder with CRC
    */
    class PolarCRCEncoder
    {
    public:
        PolarCRCEncoder(const size_t code_length, const size_t info_length, const std::vector<int>& frozen_bits, const utl::CRCHandler& crc_handler) :
            polar_encoder_{ code_length, info_length + crc_handler.num_crc_bits(), frozen_bits },
            crc_handler_{ crc_handler }
        {}

        ~PolarCRCEncoder() = default;

        Eigen::RowVectorXi encode(const Eigen::RowVectorXi& m) const
        {
            const auto& m_with_crc = crc_handler_.encode(m);
            const auto& x = polar_encoder_.encode(m_with_crc);
            return x;
        }

    private:
        PolarEncoder polar_encoder_;
        utl::CRCHandler crc_handler_;
    };
}

#endif
