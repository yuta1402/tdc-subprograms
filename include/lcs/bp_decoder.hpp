#ifndef LDPC_DECODER_HPP
#define LDPC_DECODER_HPP

#include <vector>
#include <Eigen/Core>
#include <Eigen/Sparse>

/*!
@namespace lcs
@brief LDPC Code Subprograms
*/
namespace lcs
{
    class BPDecoder
    {
    public:
        static const size_t kInitialMaxNumIterations = 20;

    public:
        BPDecoder(const Eigen::SparseMatrix<int>& parity_check_matrix, size_t max_num_iterations = kInitialMaxNumIterations);
        ~BPDecoder() = default;

        Eigen::RowVectorXi decode(const Eigen::RowVectorXd& llr);

    private:
        const Eigen::SparseMatrix<int>& parity_check_matrix_;

        size_t max_num_iterations_;

        std::vector<std::vector<size_t>> one_indices_row_;
        std::vector<std::vector<size_t>> one_indices_col_;
    };
}

#endif
