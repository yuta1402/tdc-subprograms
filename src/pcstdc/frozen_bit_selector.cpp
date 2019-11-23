#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include "estd/random.hpp"
#include "estd/parallel.hpp"
#include "estd/measure_time.hpp"
#include "pcstdc/frozen_bit_selector.hpp"
#include "pcstdc/sc_decoder.hpp"
#include "pcstdc/polar_encoder.hpp"
#include "pcstdc/info_table_handler.hpp"
#include "utl/utility.hpp"

namespace
{
    std::string generate_filename(const size_t n, const size_t num_simulation, const channel::TDCParams& tdc_params, const pcstdc::SCDecoderParams& decoder_params)
    {
        std::stringstream ss;
        ss << "td2c_capacities"
           << "_n" << n
           << "_t" << num_simulation
           << std::defaultfloat
           << "_" << tdc_params.drift_stddev
           << "_ps" << tdc_params.ps
           << "_r" << tdc_params.pass_ratio
           << "_v" << tdc_params.drift_stddev
           << "_md" << tdc_params.max_drift
           << "_seg" << decoder_params.num_segments
           << ".dat";

        return ss.str();
    }

    bool read_cache(const size_t n, const size_t num_simulation, const channel::TDCParams& tdc_params, const pcstdc::SCDecoderParams& decoder_params, std::vector<std::pair<size_t, double>>& capacities)
    {
        const std::string filename = generate_filename(n, num_simulation, tdc_params, decoder_params);
        std::ifstream ifs(filename);
        if (!ifs.is_open()) {
            return false;
        }

        for (size_t i = 0; i < n; ++i) {
            size_t j;
            double c;
            ifs >> j >> c;

            capacities[i].first = j;
            capacities[i].second = c;
        }

        ifs.close();

        return true;
    }

    bool write_cache(const size_t n, const size_t num_simulation, const channel::TDCParams& tdc_params, const pcstdc::SCDecoderParams& decoder_params, const std::vector<std::pair<size_t, double>>& capacities)
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
}

namespace pcstdc
{
    FrozenBitSelector::FrozenBitSelector(const channel::TDC& channel, const SCDecoderParams& decoder_params, const size_t num_simulation) :
        code_length_{ decoder_params.code_length },
        info_length_{ decoder_params.info_length },
        num_simulation_{ num_simulation },
        channel_{ channel },
        decoder_params_{ decoder_params },
        frozen_bits_(code_length_, 0)
    {
    }

    void FrozenBitSelector::simulate(std::vector<std::pair<size_t, double>>& capacities)
    {
        std::vector<int> tmp(code_length_, 0);

        for (size_t i = 0; i < code_length_; ++i) {
            capacities[i].first = i;
            capacities[i].second = 0.0;
        }

        for (size_t t = 0; t < num_simulation_; ++t) {
            Eigen::RowVectorXi z = Eigen::RowVectorXi::Zero(code_length_);
            for (size_t j = 0; j < code_length_; ++j) {
                z[j] = estd::Random(0, 1);
            }

            PolarEncoder encoder(code_length_, code_length_, tmp);
            SCDecoder decoder(decoder_params_, channel_, tmp);

            auto x = encoder.encode(z);
            auto y = channel_.send(x);

            InfoTableHandler u(code_length_);

            for (size_t i = 0; i < code_length_; ++i) {
                auto zz = z;
                for (int j = i; j < zz.size(); ++j) {
                    zz[j] = 0;
                }
                u.init(zz);

                const long double llg = decoder.calc_likelihood(i,   z[i], u, y);
                const long double llb = decoder.calc_likelihood(i, 1-z[i], u, y);
                const long double sum = llg + llb;

                double c = 0.0;

                if (llg != 0.0 && sum != 0.0) {
                    // 式変形: c = std::log2(llg/(0.5*llg+0.5*llb));
                    c = 1.0 + std::log2(llg) - std::log2(sum);
                    c = std::clamp(c, 0.0, 1.0);
                }

                capacities[i].second += c;
            }
        }

        for (size_t i = 0; i < code_length_; ++i) {
            capacities[i].second /= num_simulation_;
        }

        std::sort(begin(capacities), end(capacities), [](const auto& c1, const auto& c2){
            return c1.second > c2.second;
        });
    }

    void FrozenBitSelector::parallel_simulate(std::vector<std::pair<size_t, double>>& capacities, size_t num_threads)
    {
        std::vector<int> tmp(code_length_, 0);

        for (size_t i = 0; i < code_length_; ++i) {
            capacities[i].first = i;
            capacities[i].second = 0.0;
        }

        std::vector<std::vector<double>> results(num_simulation_, std::vector<double>(code_length_, 0.0));

        estd::parallel_for_each_with_reseed(results, [this, &tmp](auto&& r){
            Eigen::RowVectorXi z = Eigen::RowVectorXi::Zero(code_length_);
            for (size_t j = 0; j < code_length_; ++j) {
                z[j] = estd::Random(0, 1);
            }

            PolarEncoder encoder(code_length_, code_length_, tmp);
            SCDecoder decoder(decoder_params_, channel_, tmp);

            auto x = encoder.encode(z);
            auto y = channel_.send(x);

            InfoTableHandler u(code_length_);

            for (size_t i = 0; i < code_length_; ++i) {
                auto zz = z;
                for (int j = i; j < zz.size(); ++j) {
                    zz[j] = 0;
                }
                u.init(zz);

                const long double llg = decoder.calc_likelihood(i,   z[i], u, y);
                const long double llb = decoder.calc_likelihood(i, 1-z[i], u, y);
                const long double sum = llg + llb;

                double c = 0.0;

                if (llg != 0.0 && sum != 0.0) {
                    // 式変形: c = std::log2(llg/(0.5*llg+0.5*llb));
                    c = 1.0 + std::log2(llg) - std::log2(sum);
                    c = std::clamp(c, 0.0, 1.0);
                }

                r[i] = c;
            }
        }, num_threads);

        for (size_t t = 0; t < num_simulation_; ++t) {
            for (size_t i = 0; i < code_length_; ++i) {
                capacities[i].second += results[t][i];
            }
        }

        for (size_t i = 0; i < code_length_; ++i) {
            capacities[i].second /= num_simulation_;
        }

        estd::parallel_sort_by(capacities, [](const auto& c1, const auto& c2){
            return c1.second > c2.second;
        }, num_threads);
    }

    void FrozenBitSelector::select()
    {
        std::vector<std::pair<size_t, double>> capacities(code_length_);

        if (!read_cache(code_length_, num_simulation_, channel_.params(), decoder_params_, capacities)) {
            simulate(capacities);
            write_cache(code_length_, num_simulation_, channel_.params(), decoder_params_, capacities);
        }

        std::fill(begin(frozen_bits_), end(frozen_bits_), true);
        for (size_t i = 0; i < info_length_; ++i) {
            size_t j = capacities[i].first;
            frozen_bits_[j] = false;
        }
    }

    void FrozenBitSelector::parallel_select(size_t num_threads)
    {
        std::vector<std::pair<size_t, double>> capacities(code_length_);

        if (!read_cache(code_length_, num_simulation_, channel_.params(), decoder_params_, capacities)) {
            parallel_simulate(capacities, num_threads);
            write_cache(code_length_, num_simulation_, channel_.params(), decoder_params_, capacities);
        }

        std::fill(begin(frozen_bits_), end(frozen_bits_), true);
        for (size_t i = 0; i < info_length_; ++i) {
            size_t j = capacities[i].first;
            frozen_bits_[j] = false;
        }
    }
}
