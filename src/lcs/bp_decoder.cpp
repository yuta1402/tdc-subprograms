#include <cmath>
#include "lcs/bp_decoder.hpp"
#include "estd/random.hpp"

namespace
{
    static const double SumClampLo = -32.0;
    static const double SumClampHi = +32.0;

    static const double ProductClampLo = -(1.0 - 1e-5);
    static const double ProductClampHi = +(1.0 - 1e-5);

    // lo <= x <= hiを満たすように値を収める
    double clamp(double x, double lo, double hi)
    {
        return std::min(std::max(lo, x), hi);
    }

    void update_variable_node(
        const std::vector<std::vector<size_t>>& A,
        const std::vector<std::vector<size_t>>& B,
        const Eigen::RowVectorXd& lambda,
        const std::vector<std::vector<double>>& alpha,
        std::vector<std::vector<double>>& beta
    ) {
        for (size_t i = 0; i < A.size(); ++i) {
            for (const auto& j : A[i]) {
                double sum = 0.0;

                for (const auto& k : B[j]) {
                    if (k != i) {
                        sum += alpha[k][j];
                        sum = clamp(sum, SumClampLo, SumClampHi);
                    }
                }

                beta[j][i] = lambda[j] + sum;
                beta[j][i] = clamp(beta[j][i], SumClampLo, SumClampHi);
            }
        }
    }

    void update_check_node(
            const std::vector<std::vector<size_t>>& A,
            const std::vector<std::vector<double>>& beta,
            std::vector<std::vector<double>>& alpha
        )
    {
        for (size_t i = 0; i < A.size(); ++i) {
            for (const auto& j : A[i]) {
                double product = 1.0;

                for (const auto& k : A[i]) {
                    if (k != j) {
                        product *= std::tanh(beta[k][i] / 2.0);
                        product = clamp(product, ProductClampLo, ProductClampHi);
                    }
                }

                alpha[i][j] = 2.0 * std::atanh(product);
                alpha[i][j] = clamp(alpha[i][j], SumClampLo, SumClampHi);
            }
        }
    }

    Eigen::RowVectorXi update_temporary_predicted_codeword(
            const std::vector<std::vector<size_t>>& B,
            const Eigen::RowVectorXd& lambda,
            const std::vector<std::vector<double>>& alpha,
            std::vector<double>& gamma
        )
    {
        Eigen::RowVectorXi predicted_x(gamma.size());

        for (size_t j = 0; j < gamma.size(); ++j) {
            double sum = 0.0;

            for (const auto& k : B[j]) {
                sum += alpha[k][j];
                sum = clamp(sum, SumClampLo, SumClampHi);
            }

            gamma[j] = lambda[j] + sum;
            gamma[j] = clamp(gamma[j], SumClampLo, SumClampHi);

            if (gamma[j] > 0) {
                predicted_x[j] = 0;
            } else if (gamma[j] < 0) {
                predicted_x[j] = 1;
            } else {
                predicted_x[j] = estd::Random(0, 1);
            }
        }

        return predicted_x;
    }
}

namespace lcs
{
    BPDecoder::BPDecoder(const Eigen::SparseMatrix<int>& parity_check_matrix, size_t max_num_iterations) :
        parity_check_matrix_(parity_check_matrix),
        max_num_iterations_(max_num_iterations),
        one_indices_row_(parity_check_matrix.rows()),
        one_indices_col_(parity_check_matrix.cols())
    {
        for(Eigen::Index r = 0; r < parity_check_matrix.outerSize(); ++r) {
            for(Eigen::SparseMatrix<int>::InnerIterator it(parity_check_matrix, r); it; ++it) {
                one_indices_row_[it.row()].emplace_back(it.col());
                one_indices_col_[it.col()].emplace_back(it.row());
            }
        }
    }

    Eigen::RowVectorXi BPDecoder::decode(const Eigen::RowVectorXd& llr)
    {
        Eigen::RowVectorXi predicted_x;

        // Messages (TODO: 符号長が長くなるとメモリ使用量が大きくなるので辺ごとにメッセージを持つようにする)
        std::vector<std::vector<double>> alpha(
            parity_check_matrix_.rows(), std::vector<double>(parity_check_matrix_.cols(), 0.0)
        );

        std::vector<std::vector<double>> beta(
            parity_check_matrix_.cols(), std::vector<double>(parity_check_matrix_.rows(), 0.0)
        );

        std::vector<double> gamma(parity_check_matrix_.cols(), 0.0);

        for (size_t l = 0; l < max_num_iterations_; ++l) {
            update_variable_node(one_indices_row_, one_indices_col_, llr, alpha, beta);

            update_check_node(one_indices_row_, beta, alpha);

            predicted_x = update_temporary_predicted_codeword(one_indices_col_, llr, alpha, gamma);

            // calculate syndrome
            Eigen::RowVectorXi s = parity_check_matrix_ * predicted_x.transpose();
            for (int j = 0; j < s.size(); ++j) {
                s[j] = s[j] % 2;
            }

            if (s == Eigen::RowVectorXi::Zero(s.size())) {
                break;
            }
        }

        return predicted_x;
    }
}
