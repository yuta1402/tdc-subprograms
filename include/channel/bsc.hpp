#ifndef BSC_HPP
#define BSC_HPP

#include <Eigen/Core>

namespace channel
{
    class BSC
    {
    public:
        BSC(const double p = 0.5);
        ~BSC() = default;

        int send(const int x) const;
        Eigen::RowVectorXi send(const Eigen::RowVectorXi& x) const;

        double prob(int y, int x) const;
        double calc_capacity() const;

    private:
        double p_;
    };
}

#endif
