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
}

#include "impl/point.tcc"

#endif //EDA_POINT_HPP
