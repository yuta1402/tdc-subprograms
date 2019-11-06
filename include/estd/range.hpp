#ifndef RANGE_HPP
#define RANGE_HPP

#include <iterator>

/*!
@namespace estd
@brief extended std
*/
namespace estd
{
    /*!
    @class StepClass
    @brief range based forでステッピングを行う
    @note https://www.codeproject.com/Articles/876156/Convenient-Constructs-For-Stepping-Through-a-Range
    */
    template<class Type>
    class StepClass
    {
    public:
        class StepIterator : public std::iterator<std::forward_iterator_tag, Type>
        {
        public:
            using base       = std::iterator<std::forward_iterator_tag, Type>;
            using value_type = typename base::value_type;
            using pointer    = typename base::pointer;
            using reference  = typename base::reference;

        public:
            StepIterator(Type start, Type step, size_t index) :
                start_{ start }, step_{ step }, index_{ index }, count_{ start + static_cast<Type>(index*step) }
            {}

            ~StepIterator() = default;

            StepIterator& operator++()
            {
                ++index_;
                count_ = start_ + static_cast<Type>(index_*step_);
                return *this;
            }

            StepIterator operator++(int)
            {
                size_t tmp = index_;
                ++index_;
                count_ = start_ + static_cast<Type>(index_*step_);
                return { start_, step_, tmp };
            }

            bool operator==(const StepIterator& rhs) const { return index_ == rhs.index_; }
            bool operator!=(const StepIterator& rhs) const { return index_ != rhs.index_; }

            reference operator*() { return count_; }

        private:
            size_t index_;

            Type start_;
            Type step_;
            Type count_;
        };

        using iterator = StepIterator;

    public:
        StepClass(Type start, size_t count, Type step) :
            begin_iterator_{ start, step, 0 },
            end_iterator_{ start, step, count }
        {}

        ~StepClass() = default;

        iterator begin() const { return begin_iterator_; }
        iterator end() const { return end_iterator_; }

    private:
        iterator begin_iterator_;
        iterator end_iterator_;
    };


    template<class N>
    inline constexpr auto Step(N n)
    {
        static_assert(std::is_integral<N>::value, "Step: parameter should be of integral type");
        return StepClass<N>(N(0), n, N(1));
    }

    template<class T, class N, class S = int64_t>
    inline constexpr auto Step(T start, N count, S step = 1)
    {
        static_assert(std::is_integral<N>::value, "Step: the second parameter should be of integral type");
        return StepClass<decltype(start + step)>(start, count, step);
    }

    template<class N>
    inline constexpr auto Range(N n)
    {
        static_assert(std::is_integral<N>::value, "Range: parameter should be of integral type");
        return StepClass<N>(N(0), n, N(1));
    }

    /*!
    @fn Range
    @brief pythonライクのrange
    @todo stop-startよりstepの方が大きいときに挙動が怪しいので修正する(例外処理)
    */
    template<class T, class U, class S = int64_t, class CommonType = std::common_type_t<T, U, S>>
    inline constexpr auto Range(T start, U stop, S step = 1)
    {
        if(step == 0) {
            return StepClass<CommonType>(start, 0, step);
        }

        if((start > stop) != (step < 0)) {
            return StepClass<CommonType>(start, 0, step);
        }

        if(start == stop) {
            return StepClass<CommonType>(start, 0, step);
        }

        size_t count = 0;
        using DiffType = std::common_type_t<int64_t, CommonType>;

        S abs_step = step > 0 ? step : -step;
        CommonType diff = stop > start ? DiffType(stop) - DiffType(start) : DiffType(start) - DiffType(stop);

        if(abs_step == 1) {
            count = diff;
        } else {
            count = diff / abs_step;

            CommonType finish = start + count * step;
            if(finish != static_cast<CommonType>(stop)) {
                count++;
            }
        }

        return StepClass<CommonType>(start, count, step);
    }
}

#endif
