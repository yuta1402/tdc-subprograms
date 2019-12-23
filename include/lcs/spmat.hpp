#ifndef SPMAT_HPP
#define SPMAT_HPP

#include <string>
#include <vector>
#include <Eigen/Core>
#include <Eigen/Sparse>

/*!
@namespace lcs
@brief LDPC Code Subprograms
*/
namespace lcs
{
    struct Spmat
    {
    public:
        size_t rows;
        size_t cols;

        size_t max_num_one_row;
        size_t max_num_one_col;

        std::vector<size_t> num_one_rows;
        std::vector<size_t> num_one_cols;

        std::vector<std::vector<size_t>> one_index_rows;

    public:
        Spmat(const std::string& filename);

        Eigen::MatrixXi toMatrix() const;
        Eigen::SparseMatrix<int> toSparseMatrix() const;
    };
}

#endif
