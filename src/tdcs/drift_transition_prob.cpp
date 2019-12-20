#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "tdcs/drift_transition_prob.hpp"

namespace
{
    std::string generate_prob_table_filename(const double pass_ratio, const double drift_stddev, const int max_drift, const int num_segments)
    {
        std::stringstream ss;

        ss << "prob_table"
           << "_r" << std::scientific << std::setprecision(4) << pass_ratio
           << "_v" << std::scientific << std::setprecision(4) << drift_stddev
           << "_d" << max_drift
           << "_s" << num_segments
           << ".dat";

        return ss.str();
    }
}

namespace tdcs
{
    DriftTransitionProb::DriftTransitionProb(const double pass_ratio, const double drift_stddev, const int max_drift, const int num_segments, const std::string& prob_table_dir) :
        max_segment_{ max_drift * num_segments },
        prob_table_dir_{ prob_table_dir },
        probs_()
    {
        const auto& filename = generate_prob_table_filename(pass_ratio, drift_stddev, max_drift, num_segments);
        const auto& filepath = prob_table_dir_ + filename;

        probs_.init(-max_segment_, max_segment_, estd::nivector<double>(-max_segment_, max_segment_, 0.0));

        {
            std::ifstream ifs(filepath);
            if (!ifs.is_open()) {
                std::cerr << "read prob table error: " << filepath << std::endl;
                exit(1);
            }

            for (int i = -max_segment_; i <= max_segment_; ++i) {
                for (int j = -max_segment_; j <= max_segment_; ++j) {
                    double p;
                    ifs >> p;
                    probs_[j][i] = p;
                }
            }

            ifs.close();
        }
    }

    double DriftTransitionProb::operator()(int next_segment, int current_segment) const
    {
        if (abs(next_segment) > max_segment_) {
            return 0.0;
        }

        if (abs(current_segment) > max_segment_) {
            return 0.0;
        }

        return probs_[next_segment][current_segment];
    }
}
