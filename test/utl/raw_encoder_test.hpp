#include "test.hpp"
#include "utl/raw_encoder.hpp"

void encode_test()
{
    Eigen::RowVectorXi m(10);
    m << 1, 1, 0, 1, 0, 1, 0, 1, 1, 0;

    utl::RawEncoder encoder(m.size());
    auto x = encoder.encode(m);

    for (int i = 0; i < x.size(); ++i) {
        eassert(m[i] == x[i], "m[%d] == %d, x[%d] == %d", i, m[i], i, x[i]);
    }
}

void raw_encoder_test()
{
    encode_test();
}
