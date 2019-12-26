#include <algorithm>
#include <unordered_set>
#include <memory>
#include "pcstdc/scl_crc_decoder.hpp"

namespace
{
    struct PathWrap
    {
    public:
        Eigen::RowVectorXi path;
        pcstdc::InfoTableHandler info_table;
        std::shared_ptr<pcstdc::SCDecoder> sc_decoder;

        PathWrap(const pcstdc::SCDecoderParams& decoder_params, const channel::TDC& tdc, const std::vector<int>& frozen_bits) :
            path{ Eigen::RowVectorXi::Zero(decoder_params.code_length) },
            info_table{ decoder_params.code_length },
            sc_decoder{ std::make_shared<pcstdc::SCDecoder>(decoder_params, tdc, frozen_bits) }
        {}
    };

    struct PathLikelihoodWrap
    {
    public:
        int ui;
        long double likelihood;
        PathWrap path_wrap;

        PathLikelihoodWrap(const int ui_, const long double likelihood_, const PathWrap& path_wrap_) :
            ui{ ui_ },
            likelihood{ likelihood_ },
            path_wrap{ path_wrap_ }
        {}
    };

    Eigen::RowVectorXi extract_info_word(const size_t info_length, const Eigen::RowVectorXi& x, const std::vector<int>& frozen_bits)
    {
        Eigen::RowVectorXi m(info_length);

        size_t j = 0;
        for (int i = 0; i < x.size(); ++i) {
            if (frozen_bits[i]) {
                continue;
            }

            m[j] = x[i];
            ++j;
            if (j >= info_length) {
                break;
            }
        }

        return m;
    }
}

namespace pcstdc
{
    SCLCRCDecoder::SCLCRCDecoder(const SCDecoderParams& decoder_params, const channel::TDC& tdc, const std::vector<int>& frozen_bits, const utl::CRCHandler& crc_handler, const size_t list_size) :
        decoder_params_{ decoder_params },
        tdc_{ tdc },
        frozen_bits_{ frozen_bits },
        crc_handler_{ crc_handler },
        list_size_{ list_size }
    {
    }

    Eigen::RowVectorXi SCLCRCDecoder::decode(const Eigen::RowVectorXi& y)
    {
        auto params = decoder_params_;
        params.info_length = decoder_params_.info_length + crc_handler_.num_crc_bits();
        PathWrap init_path_wrap(params, tdc_, frozen_bits_);
        init_path_wrap.sc_decoder->init();

        std::vector<PathLikelihoodWrap> path_likelihood_wraps;
        path_likelihood_wraps.emplace_back(-1, -1, init_path_wrap);

        for (size_t i = 0; i < decoder_params_.code_length; ++i) {
            if (frozen_bits_[i]) {
                for (auto&& p : path_likelihood_wraps) {
                    p.path_wrap.path[i] = 0;
                }
            } else {
                std::vector<PathLikelihoodWrap> candidates;

                for (auto&& p : path_likelihood_wraps) {
                    const long double ll0 = p.path_wrap.sc_decoder->calc_likelihood(i, 0, p.path_wrap.info_table, y);
                    const long double ll1 = p.path_wrap.sc_decoder->calc_likelihood(i, 1, p.path_wrap.info_table, y);

                    p.path_wrap.path[i] = 0;
                    candidates.emplace_back(0, ll0, p.path_wrap);
                    p.path_wrap.path[i] = 1;
                    candidates.emplace_back(1, ll1, p.path_wrap);
                }

                std::sort(
                    std::begin(candidates),
                    std::end(candidates),
                    [](const auto& p1, const auto& p2){ return p1.likelihood > p2.likelihood; }
                );

                if (candidates.size() > list_size_) {
                    const size_t n = candidates.size() - list_size_;
                    for (size_t i = 0; i < n; ++i) {
                        candidates.pop_back();
                    }
                    candidates.shrink_to_fit();
                }

                {
                    // 同じpathから0, 1でそれぞれ派生した場合，同じSCDecoderを参照してしまうので，コピーをとる
                    std::unordered_set<std::shared_ptr<SCDecoder>> s;
                    for (auto&& p : candidates) {
                        auto it = s.find(p.path_wrap.sc_decoder);
                        if (it == s.end()) {
                            s.insert(p.path_wrap.sc_decoder);
                            continue;
                        }

                        auto ptr = *it;
                        p.path_wrap.sc_decoder = std::make_shared<SCDecoder>(*ptr);
                    }
                }

                std::swap(path_likelihood_wraps, candidates);
            }

            for (auto&& p : path_likelihood_wraps) {
                p.path_wrap.info_table.update(i, p.path_wrap.path[i]);
            }
        }

        // CRCで符号語と判断した場合はそれを返す
        for (const auto& p : path_likelihood_wraps) {
            const auto& crc_word = extract_info_word(decoder_params_.info_length + crc_handler_.num_crc_bits(), p.path_wrap.path, frozen_bits_);
            if (crc_handler_.check(crc_word)) {
                const auto& m = extract_info_word(decoder_params_.info_length, p.path_wrap.path, frozen_bits_);
                return m;
            }
        }

        // CRCで符号語が見つからない場合は，最後のビットの尤度が一番高い符号語を返す
        const auto& path = path_likelihood_wraps.front().path_wrap.path;
        const auto& m = extract_info_word(decoder_params_.info_length, path, frozen_bits_);
        return m;
    }
}
