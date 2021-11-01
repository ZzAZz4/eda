#ifndef EDA_POINT_HPP
#define EDA_POINT_HPP

#include <array>

namespace geom
{
    /* Just a point class, extend as you please */
    template<class Repr_, std::size_t Size_>
    struct Point
    {
        using value_type = Repr_;
        using size_type = std::size_t;

        std::array<value_type, Size_> _m_buff{};

        /* Number of dimensions */
        constexpr static size_type size ();

        /* Returns the value stored on dimension `dim` */
        constexpr value_type& operator [] (size_type dim);

        /* Returns the value stored on dimension `dim` */
        constexpr const value_type& operator [] (size_type dim) const;

        /* Comparison */
        constexpr bool operator == (const Point& other) const;

        /* Comparison */
        constexpr bool operator != (const Point& other) const;

        /* Iterator */
        constexpr auto begin ();

        /* Iterator */
        constexpr auto begin () const;

        /* Iterator */
        constexpr auto end ();

        /* Iterator */
        constexpr auto end () const;
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
        for (size_type i = 0; i < size(); ++i)
            if (!(this->_m_buff[i] == other._m_buff[i]))
                return false;

        return true;
    }

    template<class Repr_, std::size_t Size_>
    constexpr bool Point<Repr_, Size_>::operator != (const Point& other) const
    {
        return !operator == (other);
    }

    template<class Repr_, std::size_t Size_>
    constexpr auto
    Point<Repr_, Size_>::begin () const
    {
        return std::begin(this->_m_buff);
    }

    template<class Repr_, std::size_t Size_>
    constexpr auto
    Point<Repr_, Size_>::end () const
    {
        return std::end(this->_m_buff);
    }

    template<class Repr_, std::size_t Size_>
    constexpr auto Point<Repr_, Size_>::begin ()
    {
        return std::begin(this->_m_buff);
    }

    template<class Repr_, std::size_t Size_>
    constexpr auto Point<Repr_, Size_>::end ()
    {
        return std::end(this->_m_buff);
    }

    template<class Repr_, std::size_t Size_>
    constexpr const typename Point<Repr_, Size_>::value_type&
    Point<Repr_, Size_>::operator [] (Point::size_type dim) const
    {
        return this->_m_buff[dim];
    }

    template<class Repr_, std::size_t Size_>
    constexpr typename Point<Repr_, Size_>::value_type&
    Point<Repr_, Size_>::operator [] (Point::size_type dim)
    {
        return this->_m_buff[dim];
    }
}

#endif //EDA_POINT_HPP
