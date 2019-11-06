#ifndef COMBINATION_SOLVER_HPP
#define COMBINATION_SOLVER_HPP

/*!
@file combination_solver.hpp
@brief 組み合わせ(Combination)の計算
*/

#include <cstdint>
#include <vector>

/*!
@namespace estd
@brief extended std
*/
namespace estd
{
    class combination_solver
    {
    public:
        using value_type = int64_t;

    public:
        combination_solver(size_t size) :
            size_{ size },
            combinations(size+1, std::vector<value_type>(size+1, 0))
        {
            for (size_t i = 0; i < combinations.size(); ++i) {
                combinations[i][0] = 1;
                combinations[i][i] = 1;
            }

            for (size_t i = 1; i < combinations.size(); ++i) {
                for (size_t j = 1; j < i; ++j) {
                    combinations[i][j] = combinations[i-1][j-1] + combinations[i-1][j];
                }
            }
        }

        ~combination_solver() = default;

        value_type operator()(size_t n, size_t r)
        {
            return combinations[n][r];
        }

        size_t size() const
        {
            return size_;
        }

    private:
        size_t size_;
        std::vector<std::vector<value_type>> combinations;
    };
}

#endif
