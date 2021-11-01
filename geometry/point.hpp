#ifndef EDA_POINT_HPP
#define EDA_POINT_HPP

#include <array>
#include <algorithm>

/* Just a point class, extend as you please */
template<class Repr_, std::size_t Size_>
struct Point
{
    using value_type = Repr_;
    using size_type = std::size_t;

    template<class ... Args>
    constexpr Point (Args&& ... args)
        : m_buff{ std::forward<Args&&>(args)... } {}

    [[nodiscard]]
    constexpr static size_type size ();

    constexpr value_type& operator [] (size_type dim);

    constexpr const value_type& operator [] (size_type dim) const;

    constexpr bool operator == (const Point& other) const;

    constexpr auto begin ();

    constexpr auto begin () const;

    constexpr auto end ();

    constexpr auto end () const;

private:
    std::array<value_type, Size_> m_buff{};
};

template<class Repr_, std::size_t Size_>
constexpr typename Point<Repr_, Size_>::size_type
Point<Repr_, Size_>::size ()
{
    return Size_;
}

template<class Repr_, std::size_t Size_>
constexpr bool
Point<Repr_, Size_>::operator == (const Point& other) const
{
    return std::equal(this->begin(), this->end(), other.begin(), other.end());
}

template<class Repr_, std::size_t Size_>
constexpr auto
Point<Repr_, Size_>::begin () const
{
    return std::begin(this->m_buff);
}

template<class Repr_, std::size_t Size_>
constexpr auto
Point<Repr_, Size_>::end () const
{
    return std::end(this->m_buff);
}

template<class Repr_, std::size_t Size_>
constexpr auto Point<Repr_, Size_>::begin ()
{
    return std::begin(this->m_buff);
}

template<class Repr_, std::size_t Size_>
constexpr auto Point<Repr_, Size_>::end ()
{
    return std::end(this->m_buff);
}

template<class Repr_, std::size_t Size_>
constexpr const typename Point<Repr_, Size_>::value_type&
Point<Repr_, Size_>::operator [] (Point::size_type dim) const
{
    return this->m_buff[dim];
}

template<class Repr_, std::size_t Size_>
constexpr typename Point<Repr_, Size_>::value_type&
Point<Repr_, Size_>::operator [] (Point::size_type dim)
{
    return this->m_buff[dim];
}


#endif //EDA_POINT_HPP
