#include "pcstdc/info_table_handler.hpp"

namespace
{
    size_t calc_exponent(size_t code_length)
    {
        size_t n = 0;
        while (code_length) {
            code_length = code_length >> 1;
            ++n;
        }

        return n-1;
    }

    using container_reference = pcstdc::InfoTableHandler::container_reference;

    void init_u_rec(const int k, const int a, const int b, container_reference u)
    {
        if (k == 0) {
            return;
        }

        const int g = (a+b)/2;

        for (int j = 0; a+j < g; ++j) {
            u[k-1][a+j] = (u[k][a+2*j] + u[k][a+2*j+1]) % 2;
            u[k-1][g+j] = u[k][a+2*j+1];
        }

        init_u_rec(k-1, a, g, u);
        init_u_rec(k-1, g, b, u);
    }

    void update_u_rec(const int i, const int k, const int a, const int b, container_reference u)
    {
        if (k == 0) {
            return;
        }

        const int j = i/2;
        const int g = (a+b)/2;

        u[k-1][a+j] = (u[k][a+2*j] + u[k][a+2*j+1]) % 2;
        u[k-1][g+j] = u[k][a+2*j+1];

        update_u_rec(j, k-1, a, g, u);
        update_u_rec(j, k-1, g, b, u);
    }
}

namespace pcstdc
{
    InfoTableHandler::InfoTableHandler(const size_t code_length) :
        n_{ calc_exponent(code_length) },
        code_length_{ code_length },
        u_( n_+1, Eigen::RowVectorXi::Zero(code_length) )
    {
    }

    void InfoTableHandler::init(const Eigen::RowVectorXi& un)
    {
        u_[n_] = un;
        init_u_rec(n_, 0, code_length_, u_);
    }

    void InfoTableHandler::update(const int i, const int uni)
    {
        u_[n_][i] = uni;
        update_u_rec(i, n_, 0, code_length_, u_);
    }
}
