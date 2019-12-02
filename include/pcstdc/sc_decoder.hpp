#ifndef PCSTDC_SC_DECODER_HPP
#define PCSTDC_SC_DECODER_HPP

#include <vector>
#include <array>
#include <Eigen/Core>
#include "estd/negative_index_vector.hpp"
#include "channel/tdc.hpp"
#include "pcstdc/drift_transition_prob.hpp"
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
        using RecCalculations = std::vector<std::vector<estd::nivector<estd::nivector<std::array<RecCalculationElement, 2>>>>>;

    public:
        SCDecoder(const SCDecoderParams& params, const channel::TDC& tdc, const std::vector<int>& frozen_bits);
        ~SCDecoder() = default;

        void init();

        Eigen::RowVectorXi decode(const Eigen::RowVectorXi& z);
        long double calc_likelihood(const int i, const int ui, InfoTable& u, const Eigen::RowVectorXi& z);
        long double calc_level0(const int a, const int da, const int xa, const Eigen::RowVectorXi& z);
        long double calc_level1(const int i, const int a, const int b, const int da, const int db, InfoTable& u, const Eigen::RowVectorXi& z);
        long double calc_likelihood_rec(const int i, const int k, const int a, const int b, const int da, const int db, InfoTable& u, const Eigen::RowVectorXi& z);

    private:
        SCDecoderParams params_;
        channel::TDC tdc_;
        std::vector<int> frozen_bits_;

        DriftTransitionProb drift_transition_prob_;
        RecCalculations rec_calculations_;
    };
}

#endif
