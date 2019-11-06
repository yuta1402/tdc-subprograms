#ifndef RANDOM_HPP
#define RANDOM_HPP

/*!
@class random.hpp
@brief 標準ライブラリの<random>を拡張
*/

#include <algorithm>
#include <iterator>
#include <vector>
#include <random>
#include <unordered_map>
#include "default_random_engine.hpp"

/*!
@namespace estd
@brief extended std
*/
namespace estd
{
    inline auto Random()
    {
        return GetDefaultRandomEngine()();
    }

    template<class Type>
    inline Type Random(const Type& min, const Type& max)
    {
        return std::uniform_int_distribution<Type>(min, max)(GetDefaultRandomEngine());
    }

    template<class Type>
    inline Type Random(const Type& max)
    {
        return Random<Type>(0, max);
    }

    template<class Type>
    inline std::vector<Type> RandomMultiple(const Type& min, const Type& max, size_t num_select)
    {
        std::size_t size = std::min<std::size_t>(max - min + 1, num_select);

        std::vector<Type> tmp;
        tmp.reserve(size);

        std::unordered_map<Type, Type> conversion;

        for(size_t i = 0; i < size; ++i) {
            auto v = Random<Type>(min, max-i);
            auto it = conversion.find(v);

            Type replaced_v;
            auto replaced_it = conversion.find(max-i);

            if(replaced_it != conversion.end()) {
                replaced_v = replaced_it->second;
            } else {
                replaced_v = max-i;
            }

            if(it != conversion.end()) {
                tmp.emplace_back(it->second);
                it->second = replaced_v;
            } else {
                tmp.emplace_back(v);
                if(v != static_cast<Type>(max-i)) {
                    conversion.emplace(v, replaced_v);
                }
            }
        }

        return tmp;
    }

    template<class Type>
    inline std::vector<Type> RandomMultiple(const Type& max, size_t num_select)
    {
        return RandomMultiple<Type>(0, max, num_select);
    }

    inline bool RandomBool(const double p = 0.5)
    {
        return std::bernoulli_distribution(p)(GetDefaultRandomEngine());
    }

    template<class Iterator>
    inline auto RandomSelect(Iterator begin, Iterator end)
    {
        std::uniform_int_distribution<> dist(0, std::distance(begin, end) - 1);
        std::advance(begin, dist(GetDefaultRandomEngine()));
        return *begin;
    }

    template<class Container>
    inline auto RandomSelect(const Container& c)
    {
        return RandomSelect(std::begin(c), std::end(c));
    }

    template<class Iterator>
    inline auto RandomMultipleSelect(Iterator begin, Iterator end, size_t num_select)
    {
        using Type = typename Iterator::value_type;
        std::vector<Type> multiple;

        auto positions = RandomMultiple(std::distance(begin, end) - 1, num_select);
        for(const auto& p : positions) {
            auto it = std::next(begin, p);
            multiple.emplace_back(*it);
        }

        return multiple;
    }

    template<class Container>
    inline auto RandomMultipleSelect(const Container& c, size_t num_select)
    {
        return RandomMultipleSelect(std::begin(c), std::end(c), num_select);
    }

    template<class Iterator>
    inline void Shuffle(Iterator begin, Iterator end)
    {
        std::shuffle(begin, end, GetDefaultRandomEngine());
    }

    template<class Container>
    inline void Shuffle(Container& c)
    {
        std::shuffle(std::begin(c), std::end(c), GetDefaultRandomEngine());
    }
}

#endif
