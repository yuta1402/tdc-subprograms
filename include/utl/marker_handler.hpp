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

    private:
        Eigen::RowVectorXi marker_;
        size_t interval_;
        MarkerPriorProb prior_prob_;
    };
}

#endif
