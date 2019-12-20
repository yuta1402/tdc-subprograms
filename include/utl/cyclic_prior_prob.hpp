#ifndef CYCLIC_PRIOR_PROB_HPP
#define CYCLIC_PRIOR_PROB_HPP

#include <iostream>
#include <vector>

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

        using iterator = std::vector<double>::iterator;
        using const_iterator = std::vector<double>::const_iterator;

    public:
        CyclicPriorProb() = default;

        CyclicPriorProb(size_t cycle, double p = InitialProb) :
            cycle_{ cycle },
            probs_( cycle, p )
        {}

        CyclicPriorProb(std::initializer_list<double> list) :
            cycle_{ list.size() },
            probs_( list )
        {}

        ~CyclicPriorProb() = default;

        double& operator()(size_t i) { return probs_[i % cycle_]; }
        const double& operator()(size_t i) const { return probs_[i % cycle_]; }

        double& operator[](size_t i) { return probs_[i % cycle_]; }
        const double& operator[](size_t i) const { return probs_[i % cycle_]; }

        iterator begin() { return std::begin(probs_); }
        const_iterator begin() const { return std::begin(probs_); }

        iterator end() { return std::end(probs_); }
        const_iterator end() const { return std::end(probs_); }

        size_t cycle() const { return cycle_; }

        static CyclicPriorProb Marker01(const size_t interval)
        {
            CyclicPriorProb prob(interval*2 + 4, 0.5);
            prob[interval] = 1.0;
            prob[interval + 1] = 0.0;
            prob[interval*2 + 2] = 0.0;
            prob[interval*2 + 3] = 1.0;

            return prob;
        }

    private:
        size_t cycle_;
        std::vector<double> probs_;
    };

    inline std::ostream& operator<<(std::ostream& os, const CyclicPriorProb& p)
    {
        for (size_t i = 0; i < p.cycle(); ++i) {
            os << p[i];
            if (i < p.cycle()-1) {
                os << ' ';
            }
        }

        return os;
    }
}

#endif
