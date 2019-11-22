#include "test.hpp"
#include "utl/crc_handler.hpp"

void crc_handler_test()
{
    {
        Eigen::RowVectorXi g(3);
        g << 1, 0, 1;

        Eigen::RowVectorXi m(5);
        m << 1, 0, 0, 0, 1;

        Eigen::RowVectorXi true_x(m.size() + g.size() - 1);
        true_x << 1, 0, 0, 0, 1, 0, 0;

        Eigen::RowVectorXi error_x(m.size() + g.size() - 1);
        error_x << 1, 0, 1, 0, 1, 0, 0;

        utl::CRCHandler crc(g);
        Eigen::RowVectorXi x = crc.encode(m);
        for (int i = 0; i < x.size(); ++i) {
            eassert(x[i] == true_x[i], "x[%d] == %d, true_x[%d] == %d", i, x[i], i, true_x[i]);
        }

        eassert(crc.check(x) == true, "crc.check(x) == %d", crc.check(x));
        eassert(crc.check(error_x) == false, "crc.check(error_x) == %d", crc.check(error_x));

        eassert(crc.num_crc_bits() == static_cast<size_t>(g.size()-1)
                , "crc.num_crc_bits() == %ld, g.size()-1 == %ld", crc.num_crc_bits(), g.size()-1);
    }

    {
        Eigen::RowVectorXi g(3);
        g << 1, 0, 1;

        Eigen::RowVectorXi m(5);
        m << 1, 0, 1, 0, 1;

        Eigen::RowVectorXi true_x(m.size() + g.size() - 1);
        true_x << 1, 0, 1, 0, 1, 0, 1;

        Eigen::RowVectorXi error_x(m.size() + g.size() - 1);
        error_x << 1, 0, 1, 1, 1, 0, 0;

        utl::CRCHandler crc(g);
        Eigen::RowVectorXi x = crc.encode(m);
        for (int i = 0; i < x.size(); ++i) {
            eassert(x[i] == true_x[i], "x[%d] == %d, true_x[%d] == %d", i, x[i], i, true_x[i]);
        }

        eassert(crc.check(x) == true, "crc.check(x) == %d", crc.check(x));
        eassert(crc.check(error_x) == false, "crc.check(error_x) == %d", crc.check(error_x));

        eassert(crc.num_crc_bits() == static_cast<size_t>(g.size()-1)
                , "crc.num_crc_bits() == %ld, g.size()-1 == %ld", crc.num_crc_bits(), g.size()-1);
    }
}
