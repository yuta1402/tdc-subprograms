#include "test.hpp"
#include "utl/raw_decoder.hpp"

void decode_test()
{
    Eigen::RowVectorXi y(10);
    y << 1, 1, 0, 1, 0, 1, 0, 1, 1, 0;

    utl::RawDecoder decoder;
    auto m = decoder.decode(y);

    for (int i = 0; i < y.size(); ++i) {
        eassert(y[i] == m[i], "y[%d] == %d, m[%d] == %d", i, y[i], i, m[i]);
    }
}

void raw_decoder_test()
{
    decode_test();
}
