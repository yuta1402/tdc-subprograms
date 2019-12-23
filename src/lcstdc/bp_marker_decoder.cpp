#include "lcstdc/bp_marker_decoder.hpp"
#include "tdcs/likelihood_calculator.hpp"
#include "lcs/bp_decoder.hpp"

namespace lcstdc
{
    BPMarkerDecoder::BPMarkerDecoder(const Options& options) :
        options_{ options }
    {
    }

    Eigen::RowVectorXi BPMarkerDecoder::decode(const Eigen::RowVectorXi& z)
    {
        tdcs::LikelihoodCalculator::Params params;
        params.prior_prob = options_.marker_handler.prior_prob();
        params.num_segments = options_.num_segments;

        tdcs::LikelihoodCalculator likelihood_calculator(options_.tdc, params);

        lcs::BPDecoder bp_decoder(options_.parity_check_matrix, options_.max_num_iterations);

        auto llr_marker = likelihood_calculator.calc_llr(z);
        auto llr = options_.marker_handler.remove_marker(llr_marker);
        auto x_hat = bp_decoder.decode(llr);

        return x_hat;
    }
}
