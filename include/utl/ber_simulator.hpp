#ifndef BER_SIMULATOR_HPP
#define BER_SIMULATOR_HPP

#include <iostream>
#include <iomanip>
#include <Eigen/Core>
#include "estd/parallel.hpp"
#include "utl/utility.hpp"

/*!
@namespace utl
@brief utility
*/
namespace utl
{
    struct BERSimulatorOptions
    {
        size_t code_length;
        size_t info_length;

        size_t num_threads{ 8 };
        size_t num_epochs{ 1000 };
        size_t min_num_error_words{ 100 };
        size_t min_num_simulations{ 1000 };
        size_t max_num_simulations{ 100000 };
    };

    struct BERSimulatorResult
    {
        size_t bit_error_count{ 0 };
        size_t word_error_count{ 0 };
        size_t simulation_count{ 0 };
        double ber{ 0.0 };
        double bler{ 0.0 };
    };

    template<class Channel, class Encoder, class Decoder>
    class BERSimulator
    {
    public:
        BERSimulator(const BERSimulatorOptions& options, const Channel& channel, const Encoder& encoder, const Decoder& decoder) :
            options_{ options },
            channel_{ channel },
            encoder_{ encoder },
            decoder_{ decoder },
            result_{}
        {
        }

        ~BERSimulator() = default;

        bool step()
        {
            std::vector<size_t> distances(options_.num_epochs);

            estd::parallel_for_with_reseed(options_.num_epochs, [this, &distances](const auto& t){
                Eigen::RowVectorXi m = Eigen::RowVectorXi::Zero(options_.info_length);
                for (size_t i = 0; i < options_.info_length; ++i) {
                    m[i] = estd::Random(0, 1);
                }

                auto x = encoder_.encode(m);
                auto y = channel_.send(x);

                // 別スレッド間でメンバ変数を共有してしまう可能性があるためコピーを作成
                Decoder decoder(decoder_);
                auto m_hat = decoder.decode(y);

                Eigen::RowVectorXi e = m + m_hat;
                for (size_t i = 0; i < e.size(); ++i) {
                    e[i] = e[i] % 2;
                }

                distances[t] = e.sum();
            }, options_.num_threads);

            for (const auto& d : distances) {
                ++result_.simulation_count;

                if (d > 0) {
                    result_.bit_error_count += d;
                    ++result_.word_error_count;
                }

                if (result_.word_error_count >= options_.min_num_error_words) {
                    return true;
                }
            }

            return false;
        }

        const BERSimulatorResult& simulate()
        {
            std::cout << "simulations, wec, bec, bler, ber, progress rate" << std::endl;

            while (true) {
                if (result_.simulation_count >= options_.max_num_simulations) {
                    break;
                }

                bool is_end = step();

                result_.ber  = static_cast<double>(result_.bit_error_count) / (options_.code_length * result_.simulation_count);
                result_.bler = static_cast<double>(result_.word_error_count) / result_.simulation_count;

                auto ewc = std::min(result_.word_error_count, options_.min_num_error_words);
                double progress_rate = std::max(
                    static_cast<double>(ewc) / options_.min_num_error_words,
                    static_cast<double>(result_.simulation_count) / options_.max_num_simulations
                );

                static const std::string Delimiter = ", ";

                // TODO: 見にくいので別の関数とかクラスに切り分けるべき
                std::cout << std::setw(utl::num_digits(options_.max_num_simulations)) << std::right << result_.simulation_count
                    << Delimiter << std::setw(utl::num_digits(options_.min_num_error_words)) << std::right << result_.word_error_count
                    << Delimiter << std::setw(utl::num_digits(options_.code_length*options_.min_num_error_words)) << std::right << result_.bit_error_count
                    << std::scientific << std::setprecision(4)
                    << Delimiter << result_.bler
                    << Delimiter << result_.ber
                    << Delimiter << std::setw(3) << std::right << static_cast<int>(progress_rate * 100) << "%"
                    << std::endl;

                if (is_end) {
                    break;
                }
            }

            return result_;
        }

    private:
        const BERSimulatorOptions& options_;

        const Channel& channel_;
        const Encoder& encoder_;
        const Decoder& decoder_;

        BERSimulatorResult result_;
    };
}

#endif
