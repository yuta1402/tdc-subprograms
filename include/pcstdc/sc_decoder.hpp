#ifndef PCSTDC_SC_DECODER_HPP
#define PCSTDC_SC_DECODER_HPP

#include <vector>
#include <array>
#include <Eigen/Core>
#include "estd/negative_index_vector.hpp"
#include "channel/tdc.hpp"
#include "tdcs/drift_transition_prob.hpp"
#include "pcstdc/info_table_handler.hpp"
#include "pcstdc/rec_calculation_element.hpp"

/*!
@namespace pcstdc
@brief Polar Code Subprograms for Timing Drift Channel
*/
namespace pcstdc
{
    struct SCDecoderParams
    {
        size_t code_length;
        size_t info_length;

        int num_segments;
    };

    /*!
    @class SCDecoder
    @brief Successive Cancellation Decoder
    */
    class SCDecoder
    {
    public:
        using InfoTable = InfoTableHandler;
        using RecCalculations = std::vector<std::vector<estd::nivector<estd::nivector<RecCalculationElement>>>>;
        using Level0Calculations = std::vector<estd::nivector<std::array<long double, 2>>>;

    public:
        SCDecoder(const SCDecoderParams& params, const channel::TDC& tdc, const std::vector<int>& frozen_bits);
        ~SCDecoder() = default;

        void init();

        Eigen::RowVectorXi decode(const Eigen::RowVectorXi& z);
        std::array<long double, 2> calc_likelihood(const int i, InfoTable& u, const Eigen::RowVectorXi& z);
        std::array<long double, 2> calc_level0(const int a, const int da, const Eigen::RowVectorXi& z);
        std::array<long double, 2> calc_level0_rec(const int a, const int da, const Eigen::RowVectorXi& z);
        std::array<long double, 2> calc_level1_rec(const int i, const int a, const int b, const int da, const int db, InfoTable& u, const Eigen::RowVectorXi& z);
        std::array<long double, 2> calc_likelihood_rec(const int i, const int k, const int a, const int b, const int da, const int db, InfoTable& u, const Eigen::RowVectorXi& z);

    private:
        SCDecoderParams params_;
        channel::TDC tdc_;
        std::vector<int> frozen_bits_;

        size_t exponent_code_length_;
        int max_segment_;

        tdcs::DriftTransitionProb drift_transition_prob_;
        RecCalculations rec_calculations_;
        Level0Calculations level0_calculations_;
    };
}

#endif
