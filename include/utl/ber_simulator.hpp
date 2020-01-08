#ifndef BER_SIMULATOR_HPP
#define BER_SIMULATOR_HPP

#include <iostream>
#include <iomanip>
#include <future>
#include <vector>
#include <utility>
#include <thread>
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
            channels_( options.num_threads, channel ),
            encoders_( options.num_threads, encoder ),
            decoders_( options.num_threads, decoder ),
            result_{}
        {
        }

        ~BERSimulator() = default;

        void step()
        {
            std::vector<std::future<std::pair<size_t, size_t>>> futures;

            for (size_t t = 0; t < options_.num_threads; ++t) {
                auto seed = estd::Random();

                futures.emplace_back(std::async(std::launch::async, [this, t, seed](){
                    estd::Reseed(seed);

                    Eigen::RowVectorXi m = Eigen::RowVectorXi::Zero(options_.info_length);

                    auto&& channel = channels_[t];
                    auto&& encoder = encoders_[t];
                    auto&& decoder = decoders_[t];

                    size_t distance = 0;
                    size_t wec = 0;

                    const size_t num_tasks = (options_.num_epochs + t) / options_.num_threads;
                    for (size_t w = 0; w < num_tasks; ++w) {
                        for (size_t i = 0; i < options_.info_length; ++i) {
                            m[i] = estd::Random(0, 1);
                        }

                        auto x = encoder.encode(m);
                        auto y = channel.send(x);
                        auto m_hat = decoder.decode(y);

                        Eigen::RowVectorXi e = m + m_hat;
                        for (int i = 0; i < e.size(); ++i) {
                            e[i] = e[i] % 2;
                        }

                        distance += e.sum();
                        if (e.sum() > 0) {
                            ++wec;
                        }
                    }

                    return std::make_pair(distance, wec);
                }));
            }

            for (auto& f : futures) {
                auto [d, wec] = f.get();

                result_.bit_error_count += d;
                result_.word_error_count += wec;
            }

            result_.simulation_count += options_.num_epochs;
        }

        const BERSimulatorResult& simulate()
        {
            std::cout << "simulations, wec, bec, bler, ber, progress rate" << std::endl;

            while (true) {
                if (result_.word_error_count >= options_.min_num_error_words) {
                    break;
                }

                if (result_.simulation_count >= options_.max_num_simulations) {
                    break;
                }

                step();

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
            }

            return result_;
        }

    private:
        const BERSimulatorOptions& options_;

        std::vector<Channel> channels_;
        std::vector<Encoder> encoders_;
        std::vector<Decoder> decoders_;

        BERSimulatorResult result_;
    };
}

#endif
