#include <iostream>
#include "pcstdc/polar_encoder.hpp"
#include "estd/random.hpp"

namespace pcstdc
{
    PolarEncoder::PolarEncoder(const size_t code_length, const size_t info_length, const std::vector<int>& frozen_bits) :
        code_length_{ code_length },
        info_length_{ info_length },
        frozen_bits_{ frozen_bits }
    {
    }

    Eigen::RowVectorXi PolarEncoder::encode(const Eigen::RowVectorXi& m) const
    {
        Eigen::RowVectorXi z = Eigen::RowVectorXi::Zero(code_length_);

        int j = 0;
        for (int i = 0; i < z.size(); ++i) {
            if (frozen_bits_[i]) {
                z[i] = 0;
            } else {
                z[i] = m[j];
                ++j;
            }
        }

        for (int k = (code_length_ >> 1); k > 0; k = k >> 1) {
            Eigen::RowVectorXi tmp = Eigen::RowVectorXi::Zero(code_length_);

            for (size_t j = 0; j < code_length_; j += 2*k) {
                for (int i = 0; i < k; ++i) {
                    tmp[j + i] = z[j + 2*i] ^ z[j + 2*i + 1];
                    tmp[k + j + i] = z[j + 2*i + 1];
                }
            }

            z = tmp;
        }

        return z;
    }

    Eigen::RowVectorXi PolarEncoder::make_random_codeword() const
    {
        Eigen::RowVectorXi m = Eigen::RowVectorXi::Zero(info_length_);

        for (int i = 0; i < m.size(); ++i) {
            m[i] = estd::Random(0, 1);
        }

        auto z = encode(m);

        return z;
    }
}
