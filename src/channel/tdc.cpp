#include <iostream>
#include <random>
#include "estd/random.hpp"
#include "estd/discrete_distribution.hpp"
#include "channel/tdc.hpp"

namespace
{
    int round_drift(double d)
    {
        return std::floor(d + 0.5);
    }
}

namespace channel
{
    TDC::TDC(const TDCParams& params) :
        params_{ params }
    {}

    double TDC::generate_next_drift_value(const double di) const
    {
        if (params_.pass_ratio >= 1.0) {
            return di;
        }

        std::normal_distribution<> ndist(0.0, params_.drift_stddev);

        double r = 0.0;
        double nd = 0.0;
        while (true) {
            r = ndist(estd::GetDefaultRandomEngine());
            nd = di + r;

            const bool can_pass = abs(r) < 1.0 - params_.pass_ratio;
            const bool in_bounds = abs(nd) <= params_.max_drift;

            if (can_pass && in_bounds) {
                break;
            }
        }

        return nd;
    }

    Eigen::RowVectorXd TDC::generate_drift_sequence(const size_t code_length) const
    {
        Eigen::RowVectorXd d(code_length);
        d[0] = 0.0;

        for (int i = 0; i < d.size() - 1; ++i) {
            d[i+1] = generate_next_drift_value(d[i]);
        }

        return d;
    }

    Eigen::RowVectorXi TDC::generate_signal_sequence(const Eigen::RowVectorXi& x,const Eigen::RowVectorXd& d) const
    {
        const size_t code_length = x.size();
        Eigen::RowVectorXi y = Eigen::RowVectorXi::Constant(code_length, 2);

        for (int i = 0; i < static_cast<int>(code_length); ++i) {
            int j = i + round_drift(d[i]);

            const double left_bound  = j - 0.5*params_.pass_ratio;
            const double right_bound = j + 0.5*params_.pass_ratio;

            const bool can_pass = (left_bound < i+d[i]) && (i+d[i] < right_bound);
            const bool in_bounds = (0 <= j) && (j < static_cast<int>(code_length));

            if (can_pass && in_bounds) {
                y[j] = x[i];
            }
        }

        return y;
    }

    Eigen::RowVectorXi TDC::generate_received_word(const Eigen::RowVectorXi& y) const
    {
        const size_t code_length = y.size();
        Eigen::RowVectorXi z(code_length);

        estd::discrete_distribution<int> zero_dist({ 1.0 - params_.ps, params_.ps }, { 0, 1 });
        estd::discrete_distribution<int> one_dist({ 1.0 - params_.ps, params_.ps }, { 1, 0 });
        estd::discrete_distribution<int> erasure_dist({ 0.5, 0.5 }, {0, 1});

        for (int i = 0; i < static_cast<int>(code_length); ++i) {
            if (y[i] == 0) {
                z[i] = zero_dist();
            } else if (y[i] == 1) {
                z[i] = one_dist();
            } else {
                z[i] = erasure_dist();
            }
        }

        return z;
    }

    Eigen::RowVectorXi TDC::send(const Eigen::RowVectorXi& x) const
    {
        const size_t code_length = x.size();

        auto d = generate_drift_sequence(code_length);
        auto y = generate_signal_sequence(x, d);
        auto z = generate_received_word(y);

        return z;
    }
}
