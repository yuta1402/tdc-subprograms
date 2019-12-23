#ifndef UTL_MARKER_ENCODER_HPP
#define UTL_MARKER_ENCODER_HPP

#include "utl/marker_handler.hpp"

/*!
@namespace utl
@brief utility
*/
namespace utl
{
    class MarkerEncoder
    {
    public:
        MarkerEncoder(const utl::MarkerHandler& marker_handler) :
            marker_handler_{ marker_handler }
        {}

        ~MarkerEncoder() = default;

        Eigen::RowVectorXi encode(const Eigen::RowVectorXi& x) const
        {
            return marker_handler_.insert_marker(x);
        }

    private:
        const utl::MarkerHandler& marker_handler_;
    };
}

#endif
