#include "test.hpp"
#include "utl/cyclic_prior_prob.hpp"
#include "utl/marker_handler.hpp"
#include "utl/marker_encoder.hpp"

void marker_encoder_encode_test()
{
    auto handler = utl::MarkerHandler::Marker01(2);
    utl::MarkerEncoder encoder(handler);

    {
        Eigen::RowVectorXi x(1);
        x << 0;

        auto x_marker = encoder.encode(x);

        eassert(x_marker.size() == 1, "x_marker.size() == %ld", x_marker.size());

        eassert(x_marker[0] == 0, "x_marker[0] == %d", x_marker[0]);
    }

    {
        Eigen::RowVectorXi x(2);
        x << 0, 0;

        auto x_marker = encoder.encode(x);

        eassert(x_marker.size() == 4, "x_marker.size() == %ld", x_marker.size());

        eassert(x_marker[0] == 0, "x_marker[0] == %d", x_marker[0]);
        eassert(x_marker[1] == 0, "x_marker[1] == %d", x_marker[1]);
        eassert(x_marker[2] == 0, "x_marker[2] == %d", x_marker[2]);
        eassert(x_marker[3] == 1, "x_marker[3] == %d", x_marker[3]);
    }

    {
        Eigen::RowVectorXi x(3);
        x << 0, 0, 0;

        auto x_marker = encoder.encode(x);

        eassert(x_marker.size() == 5, "x_marker.size() == %ld", x_marker.size());

        eassert(x_marker[0] == 0, "x_marker[0] == %d", x_marker[0]);
        eassert(x_marker[1] == 0, "x_marker[1] == %d", x_marker[1]);
        eassert(x_marker[2] == 0, "x_marker[2] == %d", x_marker[2]);
        eassert(x_marker[3] == 1, "x_marker[3] == %d", x_marker[3]);
        eassert(x_marker[4] == 0, "x_marker[4] == %d", x_marker[4]);
    }

    {
        Eigen::RowVectorXi x(3);
        x << 0, 1, 0;

        auto x_marker = encoder.encode(x);

        eassert(x_marker.size() == 5, "x_marker.size() == %ld", x_marker.size());

        eassert(x_marker[0] == 0, "x_marker[0] == %d", x_marker[0]);
        eassert(x_marker[1] == 1, "x_marker[1] == %d", x_marker[1]);
        eassert(x_marker[2] == 0, "x_marker[2] == %d", x_marker[2]);
        eassert(x_marker[3] == 1, "x_marker[3] == %d", x_marker[3]);
        eassert(x_marker[4] == 0, "x_marker[4] == %d", x_marker[4]);
    }

    {
        Eigen::RowVectorXi x(3);
        x << 1, 1, 1;

        auto x_marker = encoder.encode(x);

        eassert(x_marker.size() == 5, "x_marker.size() == %ld", x_marker.size());

        eassert(x_marker[0] == 1, "x_marker[0] == %d", x_marker[0]);
        eassert(x_marker[1] == 1, "x_marker[1] == %d", x_marker[1]);
        eassert(x_marker[2] == 0, "x_marker[2] == %d", x_marker[2]);
        eassert(x_marker[3] == 1, "x_marker[3] == %d", x_marker[3]);
        eassert(x_marker[4] == 1, "x_marker[4] == %d", x_marker[4]);
    }

    {
        Eigen::RowVectorXi x(4);
        x << 0, 0, 0, 0;

        auto x_marker = encoder.encode(x);

        eassert(x_marker.size() == 8, "x_marker.size() == %ld", x_marker.size());

        eassert(x_marker[0] == 0, "x_marker[0] == %d", x_marker[0]);
        eassert(x_marker[1] == 0, "x_marker[1] == %d", x_marker[1]);
        eassert(x_marker[2] == 0, "x_marker[2] == %d", x_marker[2]);
        eassert(x_marker[3] == 1, "x_marker[3] == %d", x_marker[3]);
        eassert(x_marker[4] == 0, "x_marker[4] == %d", x_marker[4]);
        eassert(x_marker[5] == 0, "x_marker[5] == %d", x_marker[5]);
        eassert(x_marker[6] == 1, "x_marker[6] == %d", x_marker[6]);
        eassert(x_marker[7] == 0, "x_marker[7] == %d", x_marker[7]);
    }

    {
        Eigen::RowVectorXi x(4);
        x << 1, 1, 1, 1;

        auto x_marker = encoder.encode(x);

        eassert(x_marker.size() == 8, "x_marker.size() == %ld", x_marker.size());

        eassert(x_marker[0] == 1, "x_marker[0] == %d", x_marker[0]);
        eassert(x_marker[1] == 1, "x_marker[1] == %d", x_marker[1]);
        eassert(x_marker[2] == 0, "x_marker[2] == %d", x_marker[2]);
        eassert(x_marker[3] == 1, "x_marker[3] == %d", x_marker[3]);
        eassert(x_marker[4] == 1, "x_marker[4] == %d", x_marker[4]);
        eassert(x_marker[5] == 1, "x_marker[5] == %d", x_marker[5]);
        eassert(x_marker[6] == 1, "x_marker[6] == %d", x_marker[6]);
        eassert(x_marker[7] == 0, "x_marker[7] == %d", x_marker[7]);
    }
}


void marker_encoder_test()
{
    marker_encoder_encode_test();
}
