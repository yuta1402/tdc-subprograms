#ifndef PCSTDC_INFO_TABLE_HANDLER_HPP
#define PCSTDC_INFO_TABLE_HANDLER_HPP

#include <vector>
#include <Eigen/Core>

/*!
@namespace pcstdc
@brief Polar Code Subprograms for Timing Drift Channel
*/
namespace pcstdc
{
    class InfoTableHandler
    {
    public:
        using container_type      = std::vector<Eigen::RowVectorXi>;
        using container_reference = std::vector<Eigen::RowVectorXi>&;
        using reference           = typename container_type::reference;
        using const_reference     = typename container_type::const_reference;

    public:
        InfoTableHandler(const size_t code_length);
        ~InfoTableHandler() = default;

        void init(const Eigen::RowVectorXi& un);
        void update(const int i, const int uni);

        reference operator[](const size_t i) { return u_[i]; }
        const_reference operator[](const size_t i) const { return u_[i]; }

        const_reference codeword() const { return u_[0]; }
        const_reference infoword() const { return u_[n_]; }

    private:
        size_t n_;
        size_t code_length_;

        container_type u_;
    };
}

#endif
