#ifndef PARALLEL_HPP
#define PARALLEL_HPP

/*!
@class random.hpp
@brief algorithm関連の並列処理
*/

#include <algorithm>
#include <thread>
#include <future>
#include "random.hpp"

/*!
@namespace estd
@brief extended std
*/
namespace estd
{
    template<class Function>
    inline Function parallel_for(size_t n, Function f, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        if (n == 0) {
            return std::move(f);
        }

        std::size_t step = std::max<std::size_t>(1, n / num_threads);

        std::vector<std::thread> threads;

        size_t i = 0;
        for (; i < n - step; i += step) {
            threads.emplace_back([=, &f](){
                for (size_t j = i; j < i + step; ++j) {
                    f(j);
                }
            });
        }

        threads.emplace_back([=, &f](){
            for (size_t j = i; j < n; ++j) {
                f(j);
            }
        });

        for(auto&& t : threads) {
            t.join();
        }

        return std::move(f);
    }

    template<class Iterator, class Function>
    inline Function parallel_for_each(Iterator begin, Iterator end, Function f, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        if(begin == end) {
            return std::move(f);
        }

        std::size_t step = std::max<std::size_t>(1, std::distance(begin, end) / num_threads);

        std::vector<std::thread> threads;

        for(; begin < end - step; begin += step) {
            threads.emplace_back([=, &f](){
                std::for_each(begin, begin + step, f);
            });
        }

        threads.emplace_back([=, &f](){
            std::for_each(begin, end, f);
        });

        for(auto&& t : threads) {
            t.join();
        }

        return std::move(f);
    }

    template<class Container, class Function>
    inline Function parallel_for_each(Container&& c, Function f, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        return parallel_for_each(std::begin(c), std::end(c), f, num_threads);
    }

    template<class Function, class RandomEngine>
    inline Function parallel_for_with_reseed(size_t n, Function f, RandomEngine&& engine, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        if (n == 0) {
            return std::move(f);
        }

        std::size_t step = std::max<std::size_t>(1, n / num_threads);

        std::vector<std::thread> threads;

        size_t i = 0;
        for (; i < n - step; i += step) {
            auto seed = engine();
            threads.emplace_back([=, &f](){
                Reseed(seed);
                for (size_t j = i; j < i + step; ++j) {
                    f(j);
                }
            });
        }

        auto seed = engine();
        threads.emplace_back([=, &f](){
            Reseed(seed);
            for (size_t j = i; j < n; ++j) {
                f(j);
            }
        });

        for(auto&& t : threads) {
            t.join();
        }

        return std::move(f);
    }

    template<class Function>
    inline Function parallel_for_with_reseed(size_t n, Function f, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        return parallel_for_with_reseed(n, f, GetDefaultRandomEngine(), num_threads);
    }

    template<class Iterator, class Function, class RandomEngine>
    inline Function parallel_for_each_with_reseed(Iterator begin, Iterator end, Function f, RandomEngine&& engine, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        if(begin == end) {
            return std::move(f);
        }

        std::size_t step = std::max<std::size_t>(1, std::distance(begin, end) / num_threads);

        std::vector<std::thread> threads;

        for(; begin < end - step; begin += step) {
            auto seed = engine();
            threads.emplace_back([=, &f](){
                Reseed(seed);
                std::for_each(begin, begin + step, f);
            });
        }

        auto seed = engine();
        threads.emplace_back([=, &f](){
            Reseed(seed);
            std::for_each(begin, end, f);
        });

        for(auto&& t : threads) {
            t.join();
        }

        return std::move(f);
    }

    template<class Iterator, class Function>
    inline Function parallel_for_each_with_reseed(Iterator begin, Iterator end, Function f, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        return parallel_for_each_with_reseed(begin, end, f, GetDefaultRandomEngine(), num_threads);
    }

    template<class Container, class Function, class RandomEngine>
    inline Function parallel_for_each_with_reseed(Container&& c, Function f, RandomEngine&& engine, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        return parallel_for_each_with_reseed(std::begin(c), std::end(c), f, engine, num_threads);
    }

    template<class Container, class Function>
    inline Function parallel_for_each_with_reseed(Container&& c, Function f, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        return parallel_for_each_with_reseed(c, f, GetDefaultRandomEngine(), num_threads);
    }

    template<class Iterator, class Type = typename std::iterator_traits<Iterator>::value_type>
    inline Type parallel_sum(Iterator begin, Iterator end, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        if(begin == end) {
            return Type(0);
        }

        std::size_t step = std::max<std::size_t>(1, std::distance(begin, end) / num_threads);

        std::vector<std::future<Type>> futures;

        for(; begin < end - step; begin += step) {
            futures.emplace_back(std::async([=](){
                return std::accumulate(begin, begin + step, Type(0));
            }));
        }

        futures.emplace_back(std::async([=](){
            return std::accumulate(begin, end, Type(0));
        }));

        Type sum(0);

        for(auto&& f : futures) {
            sum += f.get();
        }

        return sum;
    }

    template<class Container>
    inline auto parallel_sum(const Container& c, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        return parallel_sum(std::begin(c), std::end(c), num_threads);
    }

    template<class Iterator, class Compare>
    inline void parallel_sort_by(Iterator begin, Iterator end, Compare comp, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        if (begin == end) {
            return;
        }

        std::size_t step = std::max<std::size_t>(1, std::distance(begin, end) / num_threads);

        std::vector<std::thread> threads;

        {
            auto it = begin;
            for (; it < end - step; it += step) {
                threads.emplace_back(std::thread([=](){
                    std::sort(it, it + step, comp);
                }));
            }

            threads.emplace_back(std::thread([=](){
                std::sort(it, end, comp);
            }));

            for (auto&& t : threads) {
                t.join();
            }
        }

        {
            auto it = begin + step;
            for (; it < end - step; it += step) {
                std::inplace_merge(begin, it, it + step, comp);
            }
            std::inplace_merge(begin, it, end, comp);
        }
    }

    template<class Container, class Compare>
    inline void parallel_sort_by(Container&& c, Compare comp, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        parallel_sort_by(std::begin(c), std::end(c), comp, num_threads);
    }

    template<class Iterator, class Type = typename std::iterator_traits<Iterator>::value_type>
    inline void parallel_sort(Iterator begin, Iterator end, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        parallel_sort_by(begin, end, std::less<Type>(), num_threads);
    }

    template<class Container>
    inline void parallel_sort(Container&& c, std::size_t num_threads = std::thread::hardware_concurrency())
    {
        parallel_sort(std::begin(c), std::end(c), num_threads);
    }
}

#endif
