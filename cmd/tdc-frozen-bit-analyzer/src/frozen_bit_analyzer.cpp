#include <fstream>
#include <iomanip>
#include <future>
#include <thread>
#include <vector>
#include "estd/random.hpp"
#include "estd/parallel.hpp"
#include "frozen_bit_analyzer.hpp"
#include "pcstdc/polar_encoder.hpp"
#include "pcstdc/sc_decoder.hpp"
#include "utl/utility.hpp"

#include "estd/measure_time.hpp"

namespace
{
    std::string generate_filename(const size_t n, const size_t num_simulation, const channel::TDCParams& tdc_params, const pcstdc::SCDecoderParams& decoder_params)
    {
        std::stringstream ss;
        ss << "tdc_capacities"
           << "_n" << n
           << "_t" << num_simulation
           << std::defaultfloat
           << "_ps" << tdc_params.ps
           << "_r" << tdc_params.pass_ratio
           << "_v" << tdc_params.drift_stddev
           << "_md" << tdc_params.max_drift
           << "_seg" << decoder_params.num_segments
           << ".dat";

        return ss.str();
    }

    bool save_capacity(const size_t n, const size_t num_simulation, const channel::TDCParams& tdc_params, const pcstdc::SCDecoderParams& decoder_params, const std::vector<std::pair<size_t, long double>>& capacities)
    {
        const std::string filename = generate_filename(n, num_simulation, tdc_params, decoder_params);
        std::ofstream ofs(filename, std::ios::out);

        if (!ofs.is_open()) {
            return false;
        }

        for(const auto& [ j, c ] : capacities) {
            ofs << std::setfill('0') << std::right << std::setw(utl::num_digits(n)) << j;
            ofs << ' ';
            ofs << std::scientific << std::setprecision(16) << c << '\n';
        }

        ofs << std::flush;

        ofs.close();

        return true;
    }

    std::vector<std::pair<size_t, long double>> calc_capacity(const std::vector<long double>& average_capacities)
    {
        std::vector<std::pair<size_t, long double>> capacities(average_capacities.size());

        for (size_t i = 0; i < capacities.size(); ++i) {
            capacities[i].first = i;
            capacities[i].second = average_capacities[i];
        }

        std::sort(begin(capacities), end(capacities), [](const auto& c1, const auto& c2){
            if (c1.second == c2.second) {
                return c1.first > c2.first;
            }
            return c1.second > c2.second;
        });

        return capacities;
    }

    std::vector<int> make_frozen_bits(const std::vector<std::pair<size_t, long double>>& capacities, const size_t info_length)
    {
        const size_t code_length = capacities.size();

        std::vector<int> frozen_bits(code_length, 1);
        for (size_t i = 0; i < info_length; ++i) {
            size_t j = capacities[i].first;
            frozen_bits[j] = 0;
        }

        return frozen_bits;
    }

    size_t calc_hamming_distance(const std::vector<int>& x, const std::vector<int>& y)
    {
        size_t d = 0;
        for (size_t i = 0; i < x.size(); ++i) {
            d += (x[i] + y[i]) % 2;
        }

        return d;
    }

    std::string generate_cache_filename(const size_t code_length, const size_t info_length, const channel::TDCParams& tdc_params, const pcstdc::SCDecoderParams& decoder_params)
    {
        std::stringstream ss;
        ss << "fba_cache"
           << "_n" << code_length
           << "_k" << info_length
           << std::defaultfloat
           << "_ps" << tdc_params.ps
           << "_r" << tdc_params.pass_ratio
           << "_v" << tdc_params.drift_stddev
           << "_md" << tdc_params.max_drift
           << "_seg" << decoder_params.num_segments
           << "_seed" << estd::GetDefaultRandomEngine().seed()
           << ".dat";

        return ss.str();
    }
}

FrozeBitAnalyzer::FrozeBitAnalyzer(const size_t code_length, const size_t info_length, const channel::TDC& channel, const pcstdc::SCDecoderParams& decoder_params, const size_t num_simulation, const size_t num_epoch, const size_t num_threads) :
    code_length_{ code_length },
    info_length_{ info_length },
    num_simulation_{ num_simulation },
    num_epoch_{ num_epoch },
    num_threads_{ num_threads },
    channel_{ channel },
    decoder_params_{ decoder_params },
    simulation_count_{ 0 },
    average_capacities_( code_length, 0.0 ),
    error_bit_counts_( code_length_, 0 ),
    prev_frozen_bits_( code_length_, 0 ),
    cache_filename_{ generate_cache_filename(code_length, info_length, channel.params(), decoder_params) }
{
}

void FrozeBitAnalyzer::parallel_step()
{
    const std::vector<int> tmp(code_length_, false);
    const size_t epoch_simulations = std::min(num_epoch_, num_simulation_ - simulation_count_);

    std::vector<std::future<std::pair<std::vector<long double>, std::vector<size_t>>>> futures;

    for (size_t t = 0; t < num_threads_; ++t) {
        auto seed = estd::Random();

        futures.emplace_back(std::async(std::launch::async, [this, t, seed, epoch_simulations, &tmp](){
            estd::Reseed(seed);

            Eigen::RowVectorXi z = Eigen::RowVectorXi::Zero(code_length_);

            pcstdc::PolarEncoder encoder(code_length_, code_length_, tmp);
            pcstdc::SCDecoder decoder(decoder_params_, channel_, tmp);

            std::vector<long double> sum_capacities(code_length_, 0.0);
            std::vector<size_t> error_bit_counts(code_length_, 0);

            const size_t num_tasks = (epoch_simulations + t) / num_threads_;
            for (size_t w = 0; w < num_tasks; ++w) {
                decoder.init();

                for (size_t j = 0; j < code_length_; ++j) {
                    z[j] = estd::Random(0, 1);
                }

                const auto& x = encoder.encode(z);
                const auto& y = channel_.send(x);

                pcstdc::InfoTableHandler u(code_length_);

                for (size_t i = 0; i < code_length_; ++i) {
                    auto zz = z;
                    for (int j = i; j < zz.size(); ++j) {
                        zz[j] = 0;
                    }
                    u.init(zz);

                    const auto& ll = decoder.calc_likelihood(i, u, y);
                    const long double llg = ll[z[i]];
                    const long double llb = ll[z[i] ^ 1];
                    const long double sum = llg + llb;

                    long double c = 0.0;

                    if (llg != 0.0 && sum != 0.0) {
                        // 式変形: c = std::log2(llg/(0.5*llg+0.5*llb));
                        c = 1.0 + std::log2(llg) - std::log2(sum);
                        c = std::clamp(c, 0.0l, 1.0l);
                    }

                    sum_capacities[i] += c;

                    if (z[i] == 0) {
                        if (ll[0] < ll[1]) {
                            error_bit_counts[i]++;
                        }
                    } else {
                        if (ll[1] <= ll[0]) {
                            error_bit_counts[i]++;
                        }
                    }
                }
            }
            return std::make_pair(sum_capacities, error_bit_counts);
        }));
    }

    std::vector<long double> sum_capacities(code_length_, 0.0);

    for (auto& f : futures) {
        auto [c, ebc] = f.get();
        for (size_t i = 0; i < code_length_; ++i) {
            sum_capacities[i] += c[i];
            error_bit_counts_[i] += ebc[i];
        }
    }

    for (size_t i = 0; i < code_length_; ++i) {
        const long double k = static_cast<long double>(simulation_count_) / (simulation_count_ + epoch_simulations);
        average_capacities_[i] = (k * average_capacities_[i] + sum_capacities[i] / (simulation_count_ + epoch_simulations));
    }

    simulation_count_ += epoch_simulations;
}

void FrozeBitAnalyzer::parallel_analyze()
{
    if (read_cache()) {
        std::cout << "find cache file: " << cache_filename_ << std::endl;
    }

    std::cout << "simulations, hamming distance, bec, bler, ber" << std::endl;

    while (simulation_count_ < num_simulation_) {
        parallel_step();

        const auto& capacities = calc_capacity(average_capacities_);
        auto current_frozen_bits = make_frozen_bits(capacities, info_length_);
        size_t d = calc_hamming_distance(prev_frozen_bits_, current_frozen_bits);

        size_t num_error_bits = 0;
        for (size_t i = 0; i < code_length_; ++i) {
            // 情報ビットのエラービット数を計算
            if (!current_frozen_bits[i]) {
                num_error_bits += error_bit_counts_[i];
            }
        }

        const long double bler = std::min(1.0l, static_cast<long double>(num_error_bits) / simulation_count_);
        const long double ber  = static_cast<long double>(num_error_bits) / (code_length_ * simulation_count_);

        std::cout << std::setw(utl::num_digits(num_simulation_)) << std::right << simulation_count_
            << ", " << std::setw(utl::num_digits(code_length_)) << std::right << d
            << ", " << std::setw(utl::num_digits(num_simulation_*code_length_)) << std::right << num_error_bits
            << std::scientific << std::setprecision(4)
            << ", " << bler
            << ", " << ber
            << std::endl;

        std::swap(prev_frozen_bits_, current_frozen_bits);

        save_capacity(code_length_, simulation_count_, channel_.params(), decoder_params_, capacities);
        write_cache();
    }
}

bool FrozeBitAnalyzer::read_cache()
{
    std::ifstream ifs(cache_filename_);
    if (!ifs.is_open()) {
        return false;
    }

    estd::detail::DefaultRandomEngine::result_type seed;
    size_t generator_count;
    ifs >> seed;
    ifs >> generator_count;
    estd::GetDefaultRandomEngine().reseed(seed, generator_count);

    ifs >> simulation_count_;

    for (auto&& c: average_capacities_) {
        ifs >> c;
    }

    for (auto&& e: error_bit_counts_) {
        ifs >> e;
    }

    for (auto&& f: prev_frozen_bits_) {
        ifs >> f;
    }

    ifs.close();

    return true;
}

bool FrozeBitAnalyzer::write_cache()
{
    std::ofstream ofs(cache_filename_, std::ios::out);

    if (!ofs.is_open()) {
        return false;
    }

    auto seed = estd::GetDefaultRandomEngine().seed();
    auto generator_count = estd::GetDefaultRandomEngine().generate_count();
    ofs << seed << '\n';
    ofs << generator_count << '\n';

    ofs << simulation_count_ << '\n';

    for (const auto& c: average_capacities_) {
        ofs << std::scientific << std::setprecision(16) << c << '\n';
    }

    for (const auto& e: error_bit_counts_) {
        ofs << e << '\n';
    }

    for (const auto& f: prev_frozen_bits_) {
        ofs << f << '\n';
    }

    ofs << std::flush;

    ofs.close();

    return true;
}
