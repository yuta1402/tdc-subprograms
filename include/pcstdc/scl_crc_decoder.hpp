#ifndef PCSTDC_SCL_CRC_DECODER_HPP
#define PCSTDC_SCL_CRC_DECODER_HPP

#include <vector>
#include "estd/negative_index_vector.hpp"
#include "channel/tdc.hpp"
#include "pcstdc/info_table_handler.hpp"
#include "pcstdc/rec_calculation_element.hpp"
#include "pcstdc/sc_decoder.hpp"
#include "utl/crc_handler.hpp"

/*!
@namespace pcstdc
@brief Polar Code Subprograms for Timing Drift Channel
*/
namespace pcstdc
{
    /*!
    @class SCLCRCDecoder
    @brief Successive Cancellation List Decoder with CRC
    */
    class SCLCRCDecoder
    {
    public:
        static constexpr size_t InitialListSize = 1;

    public:
        SCLCRCDecoder(const SCDecoderParams& decoder_params, const channel::TDC& tdc, const std::vector<int>& frozen_bits, const utl::CRCHandler& crc_handler, const size_t list_size = InitialListSize);
        ~SCLCRCDecoder() = default;

        Eigen::RowVectorXi decode(const Eigen::RowVectorXi& y);

    private:
        SCDecoderParams decoder_params_;
        const channel::TDC tdc_;

        std::vector<int> frozen_bits_;
        const utl::CRCHandler crc_handler_;

        size_t list_size_;
    };
}

#endif
