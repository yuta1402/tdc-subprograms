#ifndef PCSTDC_POLAR_CODE_ENCODER_HPP
#define PCSTDC_POLAR_CODE_ENCODER_HPP

#include <vector>
#include <Eigen/Core>

/*!
@namespace pcstdc
@brief Polar Code Subprograms for Timing Drift Channel
*/
namespace pcstdc
{
    class PolarCodeEncoder
    {
    public:
        PolarCodeEncoder(const size_t code_length, const size_t info_length, const std::vector<int>& frozen_bits);
        ~PolarCodeEncoder() = default;

        Eigen::RowVectorXi encode(const Eigen::RowVectorXi& m) const;
        Eigen::RowVectorXi make_random_codeword() const;

    private:
        const size_t code_length_;
        const size_t info_length_;
        const std::vector<int> frozen_bits_;
    };
}

#endif
