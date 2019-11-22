#ifndef RAW_ENCODER_HPP
#define RAW_ENCODER_HPP

#include <Eigen/Core>
#include "estd/random.hpp"

/*!
@namespace utl
@brief utility
*/
namespace utl
{
    class RawEncoder
    {
    public:
        RawEncoder(const size_t code_length) :
            code_length_{ code_length }
        {
        }

        ~RawEncoder() = default;

        Eigen::RowVectorXi encode(const Eigen::RowVectorXi& m) const
        {
            return m;
        }

        Eigen::RowVectorXi make_random_codeword() const
        {
            Eigen::RowVectorXi x = Eigen::RowVectorXi::Zero(code_length_);
            for (int i = 0; i < x.size(); ++i) {
                x[i] = estd::Random(0, 1);
            }

            return x;
        }

    private:
        const size_t code_length_;
    };
}

#endif
