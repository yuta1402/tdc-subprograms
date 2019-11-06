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
    using DefaultRandomEngineType = std::mt19937;

    inline DefaultRandomEngineType& GetDefaultRandomEngine()
    {
        static thread_local DefaultRandomEngineType engine;
        return engine;
    }

    inline void Reseed(const DefaultRandomEngineType::result_type seed)
    {
        GetDefaultRandomEngine().seed(seed);
    }
}

#endif
