#ifndef CHANNEL_TDC_HPP
#define CHANNEL_TDC_HPP

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
        double pi;
        double pd;
        double ps;

        double pass_ratio;
        double drift_stddev;

        int max_drift;
        int num_segments;
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

    private:
        TDCParams params_;
    };
}

#endif
