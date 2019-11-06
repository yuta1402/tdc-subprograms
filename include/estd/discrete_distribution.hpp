#ifndef DISCRETE_DISTRIBUTION_HPP
#define DISCRETE_DISTRIBUTION_HPP

/*!
@file discrete_distribution
@brief std::discrete_distribution<>の拡張
*/

#include <random>
#include <vector>
#include "random.hpp"

/*!
@namespace estd
@brief extended std
*/
namespace estd
{
    template<class Type> 
    class discrete_distribution
    {
    public:
        using result_type = Type;
        using result_container_type = std::vector<Type>;

    public:
        discrete_distribution(std::initializer_list<double> weight_list, std::initializer_list<result_type> result_list) :
            distribution_(weight_list),
            results_(result_list)
        {
            assert(weight_list.size() == result_list.size());
        }

        template<class WeightIterator, class ResultIterator>
        discrete_distribution(WeightIterator weight_begin, WeightIterator weight_end, ResultIterator result_begin, ResultIterator result_end) :
            distribution_(weight_begin, weight_end),
            results_(result_begin, result_end)
        {
            auto weight_distance = std::distance(weight_begin, weight_end);
            auto result_distance = std::distance(result_begin, result_end);
            assert(weight_distance == result_distance);
        }

        template<class WeightContainer, class ResultContainer>
        discrete_distribution(const WeightContainer& weight_container, const ResultContainer& result_container) :
            discrete_distribution(std::begin(weight_container), std::end(weight_container), std::begin(result_container), std::end(result_container))
        {}

        ~discrete_distribution() = default;

        std::vector<double> probabilities() const
        {
            return distribution_.probabilities();
        }

        template<class RandomEngine>
        result_type operator()(RandomEngine&& engine)
        {
            return results_[distribution_(engine)];
        }

        result_type operator()()
        {
            return operator()(GetDefaultRandomEngine());
        }

    private:
        std::discrete_distribution<std::size_t> distribution_;
        result_container_type results_;
    };
}

#endif
