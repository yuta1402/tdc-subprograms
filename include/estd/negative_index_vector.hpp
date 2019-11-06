#ifndef NEGATIVE_INDEX_VECTOR_HPP
#define NEGATIVE_INDEX_VECTOR_HPP

/*!
@file negative_index_vector.hpp
@brief std::vector<Type> with negative index
*/

#include <vector>
#include <cassert>
#include <iostream>

/*!
@namespace estd
@brief extended std
*/
namespace estd
{
    template<class Type>
    class negative_index_vector
    {
    public:
        using size_type       = int64_t;
        using value_type      = Type;
        using container_type  = std::vector<value_type>;
        using pointer         = typename container_type::pointer;
        using const_pointer   = typename container_type::const_pointer;
        using reference       = typename container_type::reference;
        using const_reference = typename container_type::const_reference;
        using iterator        = typename container_type::iterator;
        using const_iterator  = typename container_type::const_iterator;

    public:
        negative_index_vector() = default;
        negative_index_vector(const negative_index_vector& v) = default;
        negative_index_vector(negative_index_vector&& v) = default;

        negative_index_vector(size_type min, size_type max) :
            min_(min),
            max_(max),
            data_(max - min + 1)
        {
            assert(min <= max);
        }

        negative_index_vector(size_type min, size_type max, const value_type& value) :
            min_(min),
            max_(max),
            data_(max - min + 1, value)
        {
            assert(min <= max);
        }

        ~negative_index_vector() = default;

        void init(size_type min, size_type max)
        {
            assert(min <= max);

            min_ = min;
            max_ = max;

            data_ = container_type(max - min + 1);
        }

        void init(size_type min, size_type max, const value_type& value)
        {
            assert(min <= max);

            min_ = min;
            max_ = max;

            data_ = container_type(max - min + 1, value);
        }

        negative_index_vector& operator=(const negative_index_vector& rhs) = default;
        negative_index_vector& operator=(negative_index_vector&& rhs) = default;

        reference operator[](size_type index) { return data_[index - min_]; }
        const_reference operator[](size_type index) const { return data_[index - min_]; }

        iterator begin() { return std::begin(data_); }
        iterator end() { return std::end(data_); }

        const_iterator begin() const { return std::cbegin(data_); }
        const_iterator end() const { return std::cend(data_); }

        pointer data() { return data_.data(); }
        const_pointer data() const { return data_.data(); }

        size_type min() const { return min_; }
        size_type max() const { return max_; }

    private:
        size_type min_{ 0 };
        size_type max_{ 0 };
        container_type data_;
    };

    template<class Type>
    using nivector = negative_index_vector<Type>;

    template<class Type>
    inline std::ostream& operator<<(std::ostream& os, const negative_index_vector<Type>& v) {
        os << "[";

        for (int i = v.min(); i <= v.max(); ++i) {
            os << v[i];
            if (i < v.max()) {
                os << ", ";
            }
        }

        return os << "]";
    }
}

#endif
