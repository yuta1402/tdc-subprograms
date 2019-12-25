#ifndef LDPC_BER_SIMULATOR_HPP
#define LDPC_BER_SIMULATOR_HPP

#include <iostream>
#include <iomanip>
#include <Eigen/Core>
#include <Eigen/Sparse>
#include "estd/parallel.hpp"
#include "utl/utility.hpp"

/*!
@namespace lcs
@brief LDPC Code Subprograms
*/
namespace lcs
{
    namespace detail
    {
        static Eigen::RowVectorXi make_random_codeword(const Eigen::MatrixXi& h)
        {
            size_t code_length = h.cols();
            size_t num_parities = h.rows();

            Eigen::RowVectorXi c = Eigen::RowVectorXi::Zero(code_length);

            // set random information bits
            for (size_t i = num_parities; i < code_length; ++i) {
                c[i] = estd::Random(0, 1);
            }

            // calc parity bit
            for (int i = num_parities - 1; i >= 0; --i) {
                int parity = h.row(i).dot(c) % 2;
                c[i] = parity;
            }

            return c;
        }
    }

    struct LDPCBERSimulatorOptions
    {
        Eigen::MatrixXi encoder_matrix;

        size_t num_threads{ 8 };
        size_t num_epochs{ 1000 };
        size_t min_num_error_words{ 100 };
        size_t max_num_simulations{ 100000 };
    };

    struct LDPCBERSimulatorResult
    {
        size_t bit_error_count{ 0 };
        size_t word_error_count{ 0 };
        size_t simulation_count{ 0 };
        double ber{ 0.0 };
        double bler{ 0.0 };
    };

    template<class Channel, class Encoder, class Decoder>
    class LDPCBERSimulator
    {
    public:
        LDPCBERSimulator(const LDPCBERSimulatorOptions& options, const Channel& channel, const Encoder& encoder, const Decoder& decoder) :
            options_{ options },
            code_length_{ static_cast<size_t>(options.encoder_matrix.cols()) },
            channel_{ channel },
            encoder_{ encoder },
            decoder_{ decoder },
            result_{}
        {
        }

        ~LDPCBERSimulator() = default;

        bool step()
        {
            std::vector<size_t> distances(options_.num_epochs);

            estd::parallel_for_with_reseed(options_.num_epochs, [this, &distances](const auto& t){
                auto c = detail::make_random_codeword(options_.encoder_matrix);
                auto x = encoder_.encode(c);
                auto y = channel_.send(x);

                // 別スレッド間でメンバ変数を共有してしまう可能性があるためコピーを作成
                Decoder decoder(decoder_);
                auto c_hat = decoder.decode(y);

                Eigen::RowVectorXi e = c + c_hat;
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

        const LDPCBERSimulatorResult& simulate()
        {
            std::cout << "simulations, wec, bec, bler, ber, progress rate" << std::endl;

            while (true) {
                if (result_.simulation_count >= options_.max_num_simulations) {
                    break;
                }

                bool is_end = step();

                result_.ber  = static_cast<double>(result_.bit_error_count) / (code_length_ * result_.simulation_count);
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
                    << Delimiter << std::setw(utl::num_digits(code_length_*options_.min_num_error_words)) << std::right << result_.bit_error_count
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
        const LDPCBERSimulatorOptions& options_;

        const size_t code_length_;

        const Channel& channel_;
        const Encoder& encoder_;
        const Decoder& decoder_;

        LDPCBERSimulatorResult result_;
    };
}

#endif
