#ifndef CRC_HANDLER_HPP
#define CRC_HANDLER_HPP

#include <iostream>

#include <Eigen/Core>

/*!
@namespace utl
@brief utility
*/
namespace utl
{
    namespace detail
    {
        // calc the division reminder of x/y
        static Eigen::RowVectorXi calc_div_r(const Eigen::RowVectorXi& x, const Eigen::RowVectorXi& y)
        {
            Eigen::RowVectorXi r(x);

            for (int i = 0; i < r.size() - y.size() + 1; ++i) {
                if (r[i] == 0) {
                    continue;
                }

                for (int j = 0; j < y.size(); ++j) {
                    r[i+j] = (r[i+j] + y[j]) % 2;
                }
            }

            return r;
        }
    }

    /*!
    @class CRCHandler
    @brief Cyclic Redundancy Check Handler
    */
    class CRCHandler
    {
    public:
        CRCHandler(const Eigen::RowVectorXi& g) :
            g_{ g },
            num_crc_bits_{ static_cast<size_t>(g.size() - 1) }
        {
        }

        ~CRCHandler() = default;

        Eigen::RowVectorXi encode(const Eigen::RowVectorXi& m) const
        {
            Eigen::RowVectorXi x = Eigen::RowVectorXi::Zero(m.size() + g_.size() - 1);
            for (int i = 0; i < m.size(); ++i) {
                x[i] = m[i];
            }

            const auto& r = detail::calc_div_r(x, g_);

            for (int i = 0; i < g_.size()-1; ++i) {
                const int j = m.size() + i;
                x[j] = r[j];
            }

            return x;
        }

        bool check(const Eigen::RowVectorXi& x) const
        {
            const auto& r = detail::calc_div_r(x, g_);
            return r.isZero();
        }

        size_t num_crc_bits() const { return num_crc_bits_; }

    private:
        Eigen::RowVectorXi g_;
        size_t num_crc_bits_;
    };
}

#endif
