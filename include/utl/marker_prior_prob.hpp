#ifndef UTL_MARKER_PRIOR_PROB_HPP
#define UTL_MARKER_PRIOR_PROB_HPP

#include <Eigen/Core>
#include "utl/cyclic_prior_prob.hpp"

/*!
@namespace utl
@brief utility
*/
namespace utl
{
    class MarkerPriorProb : public CyclicPriorProb
    {
    public:
        MarkerPriorProb() = default;

        MarkerPriorProb(const Eigen::RowVectorXi& marker, const size_t interval) :
            CyclicPriorProb{ 2 * (marker.size() + interval), 0.5 }
        {
            for (int i = 0; i < marker.size(); ++i) {
                if (marker[i] == 0) {
                    probs_[interval + i][0] = 1.0;
                    probs_[interval + i][1] = 0.0;
                } else {
                    probs_[interval + i][0] = 0.0;
                    probs_[interval + i][1] = 1.0;
                }
            }

            for (int i = 0; i < marker.size(); ++i) {
                if (marker[marker.size() - 1 - i] == 0) {
                    probs_[2*interval + marker.size() + i][0] = 1.0;
                    probs_[2*interval + marker.size() + i][1] = 0.0;
                } else {
                    probs_[2*interval + marker.size() + i][0] = 0.0;
                    probs_[2*interval + marker.size() + i][1] = 1.0;
                }
            }
        }

        ~MarkerPriorProb() = default;

        static MarkerPriorProb Marker01(const size_t interval)
        {
            Eigen::RowVectorXi marker(2);
            marker << 0, 1;
            return MarkerPriorProb(marker, interval);
        }
    };
}

#endif
