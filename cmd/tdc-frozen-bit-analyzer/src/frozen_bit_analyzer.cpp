#include <fstream>
#include <iomanip>
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

    bool save_capacity(const size_t n, const size_t num_simulation, const channel::TDCParams& tdc_params, const pcstdc::SCDecoderParams& decoder_params, const std::vector<std::pair<size_t, double>>& capacities)
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

    std::vector<std::pair<size_t, double>> calc_capacity(const std::vector<double>& average_capacities)
    {
        std::vector<std::pair<size_t, double>> capacities(average_capacities.size());

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

    std::vector<int> make_frozen_bits(const std::vector<std::pair<size_t, double>>& capacities, const size_t info_length)
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
           << ".dat";

        return ss.str();
    }
}

FrozeBitAnalyzer::FrozeBitAnalyzer(const size_t code_length, const size_t info_length, const channel::TDC& channel, const pcstdc::SCDecoderParams& decoder_params, const size_t num_simulation, const size_t num_epoch) :
    code_length_{ code_length },
    info_length_{ info_length },
    num_simulation_{ num_simulation },
    num_epoch_{ num_epoch },
    channel_{ channel },
    decoder_params_{ decoder_params },
    simulation_count_{ 0 },
    average_capacities_( code_length, 0.0 ),
    error_bit_counts_( code_length_, 0 ),
    prev_frozen_bits_( code_length_, 0 ),
    cache_filename_{ generate_cache_filename(code_length, info_length, channel.params(), decoder_params) }
{
}

void FrozeBitAnalyzer::step()
{
    const std::vector<int> tmp(code_length_, 0);
    const size_t epoch_simulations = std::min(num_epoch_, num_simulation_ - simulation_count_);

    std::vector<double> sum_capacities(code_length_, 0.0);

    for (size_t e = 0; e < epoch_simulations; ++e) {
        Eigen::RowVectorXi z = Eigen::RowVectorXi::Zero(code_length_);
        for (size_t j = 0; j < code_length_; ++j) {
            z[j] = estd::Random(0, 1);
        }

        pcstdc::PolarEncoder encoder(code_length_, code_length_, tmp);
        pcstdc::SCDecoder decoder(decoder_params_, channel_, tmp);

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
            const double llg = ll[z[i]];
            const double llb = ll[z[i] ^ 1];
            const double sum = llg + llb;

            double c = 0.0;

            if (llg != 0.0 && sum != 0.0) {
                // 式変形: c = std::log2(llg/(0.5*llg+0.5*llb));
                c = 1.0 + std::log2(llg) - std::log2(sum);
                c = std::clamp(c, 0.0, 1.0);
            }

            sum_capacities[i] += c;
        }
    }

    for (size_t i = 0; i < code_length_; ++i) {
        const double k = static_cast<double>(simulation_count_) / (simulation_count_ + epoch_simulations);
        average_capacities_[i] = (k * average_capacities_[i] + sum_capacities[i] / (simulation_count_ + epoch_simulations));
    }

    simulation_count_ += epoch_simulations;
}

void FrozeBitAnalyzer::parallel_step(const size_t num_threads)
{
    const std::vector<int> tmp(code_length_, false);
    const size_t epoch_simulations = std::min(num_epoch_, num_simulation_ - simulation_count_);

    std::vector<double> sum_capacities(code_length_, 0.0);

    std::mutex mtx;

    estd::parallel_for_with_reseed(epoch_simulations, [this, &tmp, &sum_capacities, &mtx](const auto& e){
        Eigen::RowVectorXi z = Eigen::RowVectorXi::Zero(code_length_);
        for (size_t j = 0; j < code_length_; ++j) {
            z[j] = estd::Random(0, 1);
        }

        pcstdc::PolarEncoder encoder(code_length_, code_length_, tmp);
        pcstdc::SCDecoder decoder(decoder_params_, channel_, tmp);

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
            const double llg = ll[z[i]];
            const double llb = ll[z[i] ^ 1];
            const double sum = llg + llb;

            double c = 0.0;

            if (llg != 0.0 && sum != 0.0) {
                // 式変形: c = std::log2(llg/(0.5*llg+0.5*llb));
                c = 1.0 + std::log2(llg) - std::log2(sum);
                c = std::clamp(c, 0.0, 1.0);
            }

            {
                std::lock_guard<std::mutex> lock(mtx);
                sum_capacities[i] += c;

                if (z[i] == 0) {
                    if (ll[0] < ll[1]) {
                        error_bit_counts_[i]++;
                    }
                } else {
                    if (ll[1] <= ll[0]) {
                        error_bit_counts_[i]++;
                    }
                }
            }
        }
    }, num_threads);

    for (size_t i = 0; i < code_length_; ++i) {
        const double k = static_cast<double>(simulation_count_) / (simulation_count_ + epoch_simulations);
        average_capacities_[i] = (k * average_capacities_[i] + sum_capacities[i] / (simulation_count_ + epoch_simulations));
    }

    simulation_count_ += epoch_simulations;
}

void FrozeBitAnalyzer::analyze()
{
    std::cout << "simulations, hamming distance" << std::endl;

    while (simulation_count_ < num_simulation_) {
        step();

        const auto& capacities = calc_capacity(average_capacities_);
        auto current_frozen_bits = make_frozen_bits(capacities, info_length_);
        size_t d = calc_hamming_distance(prev_frozen_bits_, current_frozen_bits);

        std::cout << std::setw(utl::num_digits(num_simulation_)) << std::right << simulation_count_
            << ", "
            << std::setw(utl::num_digits(code_length_)) << std::right << d
            << std::endl;

        std::swap(prev_frozen_bits_, current_frozen_bits);

        save_capacity(code_length_, simulation_count_, channel_.params(), decoder_params_, capacities);
    }
}

void FrozeBitAnalyzer::parallel_analyze(const size_t num_threads)
{
    if (read_cache()) {
        std::cout << "find cache file: " << cache_filename_ << std::endl;
    }

    std::cout << "simulations, hamming distance, bec, bler, ber" << std::endl;

    while (simulation_count_ < num_simulation_) {
        parallel_step(num_threads);

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

        const double bler = std::min(1.0, static_cast<double>(num_error_bits) / simulation_count_);
        const double ber  = static_cast<double>(num_error_bits) / (code_length_ * simulation_count_);

        std::cout << std::setw(utl::num_digits(num_simulation_)) << std::right << simulation_count_
            << ", " << std::setw(utl::num_digits(code_length_)) << std::right << d
            << ", " << std::setw(utl::num_digits(num_simulation_*code_length_)) << std::right << num_error_bits
            << std::scientific << std::setprecision(4)
            << ", " << bler
            << ", " << ber
            << std::endl;

        std::swap(prev_frozen_bits_, current_frozen_bits);

        {
            estd::measure_time t("save_capacity");
            save_capacity(code_length_, simulation_count_, channel_.params(), decoder_params_, capacities);
        }
        {
            estd::measure_time t("write_cache");
            write_cache();
        }
    }
}

bool FrozeBitAnalyzer::read_cache()
{
    std::ifstream ifs(cache_filename_);
    if (!ifs.is_open()) {
        return false;
    }

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
