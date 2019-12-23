#include "test.hpp"
#include "utl/cyclic_prior_prob.hpp"
#include "utl/marker_handler.hpp"

void marker_handler_insert_marker_test()
{
    Eigen::RowVectorXi marker(2);
    marker << 0, 1;

    utl::MarkerHandler marker_handler(marker, 2);

    {
        Eigen::RowVectorXi x(3);
        x << 0, 0, 0;

        auto x_with_marker = marker_handler.insert_marker(x);

        eassert(x_with_marker.size() == 5, "x_with_marker.size() == %ld", x_with_marker.size());

        eassert(x_with_marker[0] == 0, "x_with_marker[0] == %d", x_with_marker[0]);
        eassert(x_with_marker[1] == 0, "x_with_marker[1] == %d", x_with_marker[1]);
        eassert(x_with_marker[2] == 0, "x_with_marker[2] == %d", x_with_marker[2]);
        eassert(x_with_marker[3] == 1, "x_with_marker[3] == %d", x_with_marker[3]);
        eassert(x_with_marker[4] == 0, "x_with_marker[4] == %d", x_with_marker[4]);
    }

    {
        Eigen::RowVectorXi x(3);
        x << 0, 1, 0;

        auto x_with_marker = marker_handler.insert_marker(x);

        eassert(x_with_marker.size() == 5, "x_with_marker.size() == %ld", x_with_marker.size());

        eassert(x_with_marker[0] == 0, "x_with_marker[0] == %d", x_with_marker[0]);
        eassert(x_with_marker[1] == 1, "x_with_marker[1] == %d", x_with_marker[1]);
        eassert(x_with_marker[2] == 0, "x_with_marker[2] == %d", x_with_marker[2]);
        eassert(x_with_marker[3] == 1, "x_with_marker[3] == %d", x_with_marker[3]);
        eassert(x_with_marker[4] == 0, "x_with_marker[4] == %d", x_with_marker[4]);
    }

    {
        Eigen::RowVectorXi x(3);
        x << 1, 1, 1;

        auto x_with_marker = marker_handler.insert_marker(x);

        eassert(x_with_marker.size() == 5, "x_with_marker.size() == %ld", x_with_marker.size());

        eassert(x_with_marker[0] == 1, "x_with_marker[0] == %d", x_with_marker[0]);
        eassert(x_with_marker[1] == 1, "x_with_marker[1] == %d", x_with_marker[1]);
        eassert(x_with_marker[2] == 0, "x_with_marker[2] == %d", x_with_marker[2]);
        eassert(x_with_marker[3] == 1, "x_with_marker[3] == %d", x_with_marker[3]);
        eassert(x_with_marker[4] == 1, "x_with_marker[4] == %d", x_with_marker[4]);
    }

    {
        Eigen::RowVectorXi x(4);
        x << 0, 0, 0, 0;

        auto x_with_marker = marker_handler.insert_marker(x);

        eassert(x_with_marker.size() == 8, "x_with_marker.size() == %ld", x_with_marker.size());

        eassert(x_with_marker[0] == 0, "x_with_marker[0] == %d", x_with_marker[0]);
        eassert(x_with_marker[1] == 0, "x_with_marker[1] == %d", x_with_marker[1]);
        eassert(x_with_marker[2] == 0, "x_with_marker[2] == %d", x_with_marker[2]);
        eassert(x_with_marker[3] == 1, "x_with_marker[3] == %d", x_with_marker[3]);
        eassert(x_with_marker[4] == 0, "x_with_marker[4] == %d", x_with_marker[4]);
        eassert(x_with_marker[5] == 0, "x_with_marker[5] == %d", x_with_marker[5]);
        eassert(x_with_marker[6] == 1, "x_with_marker[6] == %d", x_with_marker[6]);
        eassert(x_with_marker[7] == 0, "x_with_marker[7] == %d", x_with_marker[7]);
    }

    {
        Eigen::RowVectorXi x(4);
        x << 1, 1, 1, 1;

        auto x_with_marker = marker_handler.insert_marker(x);

        eassert(x_with_marker.size() == 8, "x_with_marker.size() == %ld", x_with_marker.size());

        eassert(x_with_marker[0] == 1, "x_with_marker[0] == %d", x_with_marker[0]);
        eassert(x_with_marker[1] == 1, "x_with_marker[1] == %d", x_with_marker[1]);
        eassert(x_with_marker[2] == 0, "x_with_marker[2] == %d", x_with_marker[2]);
        eassert(x_with_marker[3] == 1, "x_with_marker[3] == %d", x_with_marker[3]);
        eassert(x_with_marker[4] == 1, "x_with_marker[4] == %d", x_with_marker[4]);
        eassert(x_with_marker[5] == 1, "x_with_marker[5] == %d", x_with_marker[5]);
        eassert(x_with_marker[6] == 1, "x_with_marker[6] == %d", x_with_marker[6]);
        eassert(x_with_marker[7] == 0, "x_with_marker[7] == %d", x_with_marker[7]);
    }
}

void marker_handler_test()
{
    marker_handler_insert_marker_test();
}
