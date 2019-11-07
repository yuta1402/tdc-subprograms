#ifndef CHANNEL_TDC_HPP
#define CHANNEL_TDC_HPP

#include <Eigen/Core>

/*!
@namespace channel
@brief channel
*/
namespace channel
{
    /*!
    @class TDCParams
    @brief the parameters of TDC
    */
    struct TDCParams
    {
        double ps;
        double pass_ratio;
        double drift_stddev;

        int max_drift;
    };

    /*!
    @class TDC
    @brief Timing Drift Channel
    */
    class TDC
    {
    public:
        TDC(const TDCParams& params);
        ~TDC() = default;

        double generate_next_drift_value(const double di);
        Eigen::RowVectorXd generate_drift_sequence(const size_t code_length);
        Eigen::RowVectorXi generate_signal_sequence(const Eigen::RowVectorXi& x,const Eigen::RowVectorXd& d);
        Eigen::RowVectorXi generate_received_word(const Eigen::RowVectorXi& y);

        Eigen::RowVectorXi send(const Eigen::RowVectorXi& x);

    private:
        TDCParams params_;
    };
}

#endif
