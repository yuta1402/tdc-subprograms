#ifndef UTL_MARKER_HANDLER_HPP
#define UTL_MARKER_HANDLER_HPP

#include <Eigen/Core>
#include "utl/cyclic_prior_prob.hpp"
#include "utl/marker_prior_prob.hpp"

/*!
@namespace utl
@brief utility
*/
namespace utl
{
    class MarkerHandler
    {
    public:
        MarkerHandler(const Eigen::RowVectorXi& marker, const size_t interval) :
            marker_{ marker },
            interval_{ interval },
            prior_prob_{ marker, interval }
        {
        }

        ~MarkerHandler() = default;

        template<class Type>
        Type insert_marker(const Type& x)
        {
            const size_t num_markers = x.size() / interval_;
            const size_t length = x.size() + num_markers * marker_.size();
            Type x_marker(length);

            int j = 0;
            for (int i = 0; i < x_marker.size(); ++i) {
                if (prior_prob_[i][0] == 1.0) {
                    x_marker[i] = 0;
                    continue;
                }

                if (prior_prob_[i][1] == 1.0) {
                    x_marker[i] = 1;
                    continue;
                }

                x_marker[i] = x[j];
                ++j;
            }

            return x_marker;
        }

        template<class Type> 
        Type remove_marker(const Type& x_marker)
        {
            const size_t num_markers = x_marker.size() / (interval_ + marker_.size());
            const size_t length = x_marker.size() - num_markers * marker_.size();
            Type x(length);

            int j = 0;
            for (int i = 0; i < x_marker.size(); ++i) {
                if (prior_prob_[i][0] == 1.0) {
                    continue;
                }

                if (prior_prob_[i][1] == 1.0) {
                    continue;
                }

                x[j] = x_marker[i];
                ++j;
            }

            return x;
        }

    private:
        Eigen::RowVectorXi marker_;
        size_t interval_;
        MarkerPriorProb prior_prob_;
    };
}

#endif
