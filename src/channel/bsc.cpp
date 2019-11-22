#include <iostream>
#include <cmath>
#include "channel/bsc.hpp"
#include "estd/random.hpp"

namespace
{
    double binary_entropy_function(const double p)
    {
        if (p == 0.0 || p == 1.0) {
            return 0.0;
        }

        return - p*std::log2(p) - (1-p)*std::log2(1-p);
    }
}

namespace channel
{
    BSC::BSC(const double p) :
        p_{ p }
    {}

    int BSC::send(const int x) const
    {
        if (estd::RandomBool(p_)) {
            return 1 - x;
        }

        return x;
    }

    Eigen::RowVectorXi BSC::send(const Eigen::RowVectorXi& x) const
    {
        Eigen::RowVectorXi y(x.size());

        for (int i = 0; i < x.size(); ++i) {
            if (estd::RandomBool(p_)) {
                y[i] = 1 - x[i];
            } else {
                y[i] = x[i];
            }
        }

        return y;
    }

    double BSC::prob(int y, int x) const
    {
        if (y == 0 && x == 0) {
            return 1.0 - p_;
        } else if (y == 0 && x == 1) {
            return p_;
        } else if (y == 1 && x == 0) {
            return p_;
        } else if (y == 1 && x == 1) {
            return 1.0 - p_;
        }

        return 0.0;
    }

    double BSC::calc_capacity() const
    {
        double h = binary_entropy_function(p_);
        return 1 - h;
    }
}
