#include <vector>
#include "test.hpp"
#include "pcstdc/polar_encoder.hpp"

void polar_encoder_encode_test()
{
    std::vector<int> frozen_bits{ 0, 0, 1, 0, 1, 1, 1, 0 };
    size_t n = 8;
    size_t k = 4;

    pcstdc::PolarEncoder encoder(n, k, frozen_bits);

    Eigen::RowVectorXi m(k);
    m << 1, 0, 1, 1;

    auto x = encoder.encode(m);
    Eigen::RowVectorXi true_x(n);
    true_x << 1, 1, 0, 1, 0, 1, 0, 1;

    for (int i = 0; i < x.size(); ++i) {
        eassert(x[i] == true_x[i], "x[%d] == %d, true_x[%d] == %d", i, x[i], i, true_x[i]);
    }
}

void polar_encoder_test()
{
    polar_encoder_encode_test();
}
