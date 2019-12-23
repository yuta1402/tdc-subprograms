#ifndef CYCLIC_PRIOR_PROB_HPP
#define CYCLIC_PRIOR_PROB_HPP

#include <iostream>
#include <vector>
#include <array>

/*!
@namespace utl
@brief utility
*/
namespace utl
{
    struct CyclicPriorProb
    {
    public:
        static constexpr double InitialProb = 0.5;

        using value_type = std::array<double, 2>;
        using iterator = std::vector<value_type>::iterator;
        using const_iterator = std::vector<value_type>::const_iterator;

    public:
        CyclicPriorProb() = default;

        CyclicPriorProb(size_t cycle, double p0 = InitialProb) :
            CyclicPriorProb{ cycle, { p0, 1.0 - p0 } }
        {}

        CyclicPriorProb(size_t cycle, const value_type& p) :
            cycle_{ cycle },
            probs_( cycle, p )
        {}

        CyclicPriorProb(std::initializer_list<value_type> list) :
            cycle_{ list.size() },
            probs_( list )
        {}

        ~CyclicPriorProb() = default;

        value_type& operator()(size_t i) { return probs_[i % cycle_]; }
        const value_type& operator()(size_t i) const { return probs_[i % cycle_]; }

        value_type& operator[](size_t i) { return probs_[i % cycle_]; }
        const value_type& operator[](size_t i) const { return probs_[i % cycle_]; }

        iterator begin() { return std::begin(probs_); }
        const_iterator begin() const { return std::begin(probs_); }

        iterator end() { return std::end(probs_); }
        const_iterator end() const { return std::end(probs_); }

        size_t cycle() const { return cycle_; }

        static CyclicPriorProb Marker01(const size_t interval)
        {
            CyclicPriorProb prob(interval*2 + 4, 0.5);
            prob[interval][0] = 1.0;
            prob[interval + 1][0] = 0.0;
            prob[interval*2 + 2][0] = 0.0;
            prob[interval*2 + 3][0] = 1.0;

            prob[interval][1] = 0.0;
            prob[interval + 1][1] = 1.0;
            prob[interval*2 + 2][1] = 1.0;
            prob[interval*2 + 3][1] = 0.0;

            return prob;
        }

    protected:
        size_t cycle_;
        std::vector<value_type> probs_;
    };

    inline std::ostream& operator<<(std::ostream& os, const CyclicPriorProb& p)
    {
        for (size_t i = 0; i < p.cycle(); ++i) {
            os << "{ " << p[i][0] << ", " << p[i][1] << " }";
            if (i < p.cycle()-1) {
                os << ", ";
            }
        }

        return os;
    }
}

#endif
