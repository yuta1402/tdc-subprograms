#ifndef DEFAULT_RANDOM_ENGINE_HPP
#define DEFAULT_RANDOM_ENGINE_HPP

/*!
@file default_random_engine.hpp
@brief グローバルの乱数生成エンジン
*/

#include <random>

/*!
@namespace estd
@brief extended std
*/
namespace estd
{
    namespace detail
    {
        class DefaultRandomEngine
        {
        public:
            using engine_type = std::mt19937;
            using result_type = engine_type::result_type;

            static constexpr const result_type default_seed = engine_type::default_seed;

        public:
            DefaultRandomEngine(result_type seed = default_seed, std::size_t generate_count = 0) :
                seed_{ seed },
                engine_{ seed },
                generate_count_{ generate_count }
            {
                engine_.discard(generate_count_);
            }

            ~DefaultRandomEngine() = default;

            static constexpr result_type min() { return engine_type::min(); }
            static constexpr result_type max() { return engine_type::max(); }

            void reseed(result_type seed = default_seed, std::size_t generate_count = 0)
            {
                seed_ = seed;
                engine_.seed(seed);
                generate_count_ = generate_count;
                engine_.discard(generate_count_);
            }

            result_type operator()()
            {
                const result_type r = engine_();
                ++generate_count_;
                return r;
            }

            result_type seed() const { return seed_; }
            std::size_t generate_count() const { return generate_count_; }

        private:
            result_type seed_;
            engine_type engine_;
            std::size_t generate_count_;
        };
    }

    inline detail::DefaultRandomEngine& GetDefaultRandomEngine()
    {
        static thread_local detail::DefaultRandomEngine engine;
        return engine;
    }

    inline void Reseed(const detail::DefaultRandomEngine::result_type seed)
    {
        GetDefaultRandomEngine().reseed(seed);
    }
}

#endif
