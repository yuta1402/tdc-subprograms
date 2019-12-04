#include <fstream>
#include <iomanip>
#include "estd/random.hpp"
#include "estd/parallel.hpp"
#include "frozen_bit_analyzer.hpp"
#include "pcstdc/polar_encoder.hpp"
#include "pcstdc/sc_decoder.hpp"
#include "utl/utility.hpp"

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
            ofs << std::scientific << std::setprecision(16) << c << std::endl;
        }

        ofs.close();

        return true;
    }

    std::vector<std::pair<size_t, long double>> calc_capacity(const std::vector<std::pair<size_t, long double>>& sum_capacities, const size_t num_simulation)
    {
        auto capacities = sum_capacities;

        for (size_t i = 0; i < capacities.size(); ++i) {
            capacities[i].second /= num_simulation;
        }

        std::sort(begin(capacities), end(capacities), [](const auto& c1, const auto& c2){
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
}

FrozeBitAnalyzer::FrozeBitAnalyzer(const size_t code_length, const size_t info_length, const channel::TDC& channel, const pcstdc::SCDecoderParams& decoder_params, const size_t num_simulation, const size_t num_epoch) :
    code_length_{ code_length },
    info_length_{ info_length },
    num_simulation_{ num_simulation },
    num_epoch_{ num_epoch },
    channel_{ channel },
    decoder_params_{ decoder_params },
    simulation_count_{ 0 },
    sum_capacities_( code_length )
{
    for (size_t i = 0; i < sum_capacities_.size(); ++i) {
        sum_capacities_[i].first = i;
        sum_capacities_[i].second = 0.0;
    }
}

void FrozeBitAnalyzer::step()
{
    const std::vector<int> tmp(code_length_, 0);
    const size_t epoch_simulations = std::min(num_epoch_, num_simulation_ - simulation_count_);

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

            const long double llg = decoder.calc_likelihood(i,   z[i], u, y);
            const long double llb = decoder.calc_likelihood(i, 1-z[i], u, y);
            const long double sum = llg + llb;

            long double c = 0.0;

            if (llg != 0.0 && sum != 0.0) {
                // 式変形: c = std::log2(llg/(0.5*llg+0.5*llb));
                c = 1.0 + std::log2(llg) - std::log2(sum);
                c = std::clamp(c, 0.0l, 1.0l);
            }

            sum_capacities_[i].second += c;
        }
    }

    simulation_count_ += epoch_simulations;
}

void FrozeBitAnalyzer::parallel_step(const size_t num_threads)
{
    const std::vector<int> tmp(code_length_, false);
    const size_t epoch_simulations = std::min(num_epoch_, num_simulation_ - simulation_count_);

    std::mutex mtx;

    estd::parallel_for_with_reseed(epoch_simulations, [this, &tmp, &mtx](const auto& e){
        Eigen::RowVectorXi z = Eigen::RowVectorXi::Zero(code_length_);
        for (size_t j = 0; j < code_length_; ++j) {
            z[j] = estd::Random(0, 1);
        }

        pcs::PolarCodeEncoder encoder(code_length_, code_length_, tmp);
        pctd2cs::SCDecoder decoder(code_length_, code_length_, tmp, channel_, alpha_);

        const auto& x = encoder.encode(z);
        const auto& y = channel_.send(x);

        pcs::InfoTableHandler u(code_length_);

        for (size_t i = 0; i < code_length_; ++i) {
            auto zz = z;
            for (int j = i; j < zz.size(); ++j) {
                zz[j] = 0;
            }
            u.init(zz);

            const long double llg = decoder.calc_likelihood(i,   z[i], u, y);
            const long double llb = decoder.calc_likelihood(i, 1-z[i], u, y);
            const long double sum = llg + llb;

            long double c = 0.0;

            if (llg != 0.0 && sum != 0.0) {
                // 式変形: c = std::log2(llg/(0.5*llg+0.5*llb));
                c = 1.0 + std::log2(llg) - std::log2(sum);
                c = std::clamp(c, 0.0l, 1.0l);
            }

            {
                std::lock_guard<std::mutex> lock(mtx);
                sum_capacities_[i].second += c;
            }
        }
    }, num_threads);

    simulation_count_ += epoch_simulations;
}

void FrozeBitAnalyzer::analyze()
{
    std::vector<int> prev_frozen_bits(code_length_, 0);

    std::cout << "simulations, hamming distance" << std::endl;

    while (simulation_count_ < num_simulation_) {
        step();

        const auto& capacities = calc_capacity(sum_capacities_, simulation_count_);
        auto current_frozen_bits = make_frozen_bits(capacities, info_length_);
        size_t d = calc_hamming_distance(prev_frozen_bits, current_frozen_bits);

        std::cout << std::setw(utl::num_digits(num_simulation_)) << std::right << simulation_count_
            << ", "
            << std::setw(utl::num_digits(code_length_)) << std::right << d
            << std::endl;

        std::swap(prev_frozen_bits, current_frozen_bits);

        save_capacity(code_length_, simulation_count_, channel_.params(), alpha_, capacities);
    }
}

void FrozeBitAnalyzer::parallel_analyze(const size_t num_threads)
{
    std::vector<int> prev_frozen_bits(code_length_, 0);

    std::cout << "simulations, hamming distance" << std::endl;

    while (simulation_count_ < num_simulation_) {
        parallel_step(num_threads);

        const auto& capacities = calc_capacity(sum_capacities_, simulation_count_);
        auto current_frozen_bits = make_frozen_bits(capacities, info_length_);
        size_t d = calc_hamming_distance(prev_frozen_bits, current_frozen_bits);

        std::cout << std::setw(utl::num_digits(num_simulation_)) << std::right << simulation_count_
            << ", "
            << std::setw(utl::num_digits(code_length_)) << std::right << d
            << std::endl;

        std::swap(prev_frozen_bits, current_frozen_bits);

        save_capacity(code_length_, simulation_count_, channel_.params(), alpha_, capacities);
    }
}
