#ifndef LCSTDC_BP_MARKER_DECODER_HPP
#define LCSTDC_BP_MARKER_DECODER_HPP

#include <Eigen/Sparse>
#include "utl/marker_handler.hpp"
#include "channel/tdc.hpp"

/*!
@namespace lcstdc
@brief LDPC Code Subprograms for Timing Drift Channel
*/
namespace lcstdc
{
    class BPMarkerDecoder
    {
    public:
        struct Options
        {
            channel::TDC tdc;

            utl::MarkerHandler marker_handler;

            Eigen::SparseMatrix<int> parity_check_matrix;

            int num_segments;
            size_t max_num_iterations;
        };

    public:
        BPMarkerDecoder(const Options& options);
        ~BPMarkerDecoder() = default;

        Eigen::RowVectorXi decode(const Eigen::RowVectorXi& z);

    private:
        Options options_;
    };
}

#endif
