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
        using InfoTable = InfoTableHandler;
        using InfoTables = std::vector<InfoTable>;
        using RecCalculations = std::vector<std::vector<std::vector<RecCalculationElement>>>;
        using Level0Calculations = std::vector<estd::nivector<std::array<long double, 2>>>;

        static constexpr size_t InitialListSize = 1;

    public:
        SCLCRCDecoder(const SCDecoderParams& decoder_params, const channel::TDC& tdc, const std::vector<int>& frozen_bits, const utl::CRCHandler& crc_handler, const size_t list_size = InitialListSize);
        ~SCLCRCDecoder() = default;

        void init();

        Eigen::RowVectorXi decode(const Eigen::RowVectorXi& z);
        std::vector<std::array<long double, 2>> calc_likelihood(const int i, InfoTables& u, const Eigen::RowVectorXi& z);
        std::array<long double, 2> calc_level0(const int a, const int da, const Eigen::RowVectorXi& z);
        estd::nivector<std::array<long double, 2>> calc_level0_rec(const int a, const Eigen::RowVectorXi& z);
        void calc_level1_rec(const int i, const int a, const int b, InfoTables& u, const Eigen::RowVectorXi& z);
        void calc_likelihood_rec(const int i, const int k, const int a, const int b, InfoTables& u, const Eigen::RowVectorXi& z);

    private:
        SCDecoderParams decoder_params_;
        const channel::TDC tdc_;
        std::vector<int> frozen_bits_;

        size_t exponent_code_length_;
        int max_segment_;

        tdcs::DriftTransitionProb drift_transition_prob_;
        RecCalculations rec_calculations_;
        Level0Calculations level0_calculations_;

        const utl::CRCHandler crc_handler_;
        size_t list_size_;
    };
}

#endif
