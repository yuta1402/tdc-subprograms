#ifndef RAW_DECODER_HPP
#define RAW_DECODER_HPP

#include <Eigen/Core>
#include "channel/bsc.hpp"

/*!
@namespace utl
@brief utility
*/
namespace utl
{
    class RawDecoder
    {
    public:
        RawDecoder()
        {
        }

        ~RawDecoder() = default;

        Eigen::RowVectorXi decode(const Eigen::RowVectorXi& y) const
        {
            return y;
        }
    };
}

#endif
