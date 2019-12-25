#include <iostream>
#include <fstream>
#include "lcs/spmat.hpp"

namespace lcs
{
    Spmat::Spmat(const std::string& filename)
    {
        std::ifstream spmat_file(filename, std::ios::in);
        if (!spmat_file.is_open()) {
            std::cerr << "read spmat file error: " << filename << std::endl;
            exit(1);
        }

        spmat_file >> cols >> rows;
        spmat_file >> max_num_one_row >> max_num_one_col;

        num_one_rows.resize(rows);
        num_one_cols.resize(cols);

        for (auto&& num : num_one_rows) {
            spmat_file >> num;
        }

        for (auto&& num : num_one_cols) {
            spmat_file >> num;
        }

        one_index_rows.resize(rows);

        for (size_t r = 0; r < num_one_rows.size(); ++r) {
            for (size_t i = 0; i < num_one_rows[r]; ++i ) {
                size_t c;
                spmat_file >> c;
                --c;

                one_index_rows[r].emplace_back(c);
            }
        }
    }

    Eigen::MatrixXi Spmat::toMatrix() const
    {
        Eigen::MatrixXi m = Eigen::MatrixXi::Zero(rows, cols);

        for (size_t r = 0; r < one_index_rows.size(); ++r) {
            for (const auto& c : one_index_rows[r]) {
                m(r, c) = 1;
            }
        }

        return m;
    }

    Eigen::SparseMatrix<int> Spmat::toSparseMatrix() const
    {
        Eigen::SparseMatrix<int> m(rows, cols);

        for (size_t r = 0; r < one_index_rows.size(); ++r) {
            for (const auto& c : one_index_rows[r]) {
                m.insert(r, c) = 1;
            }
        }

        return m;
    }
}
