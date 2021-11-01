#ifndef EDA_BOX_HPP
#define EDA_BOX_HPP

#include "point.hpp"

namespace geom
{
    /* Implementation details */
    namespace detail
    {
        /* Returns a point left-enclosing a region defined by two points */
        template<class Repr_, std::size_t Size_>
        constexpr Point<Repr_, Size_> lower_limits (
            const Point<Repr_, Size_>& lhs,
            const Point<Repr_, Size_>& rhs)
        {
            Point<Repr_, Size_> result;
            for (std::size_t i = 0; i < Size_; ++i)
                result[i] = std::min(lhs[i], rhs[i]);

            return result;
        }

        /* Returns a point right-enclosing a region defined by two points */
        template<class Repr_, std::size_t Size_>
        constexpr Point<Repr_, Size_> upper_limits (
            const Point<Repr_, Size_>& lhs,
            const Point<Repr_, Size_>& rhs)
        {
            Point<Repr_, Size_> result;
            for (std::size_t i = 0; i < Size_; ++i)
                result[i] = std::max(lhs[i], rhs[i]);

            return result;
        }
    }

    template<class Repr_, std::size_t Size_>
    struct Box
    {
        using point_type = Point<Repr_, Size_>;
        using size_type = typename point_type::size_type;
        using value_type = typename point_type::value_type;

        /* const so that nobody does something stupid with the ring */
        /* lower point on the box on all dimensions */
        const point_type lower;

        /* upper point on the box on all dimensions */
        const point_type upper;

        /* Constructor.
         * Creates a bbox that encloses both lower and upper,
         * regardless of order on each dimension */
        constexpr Box (point_type lower_, point_type upper_);

        /* Comparison */
        constexpr bool operator == (const Box& other) const;

        /* Comparison */
        constexpr bool operator != (const Box& other) const;
    };

    template<class Repr_, std::size_t Size_>
    constexpr
    Box<Repr_, Size_>::Box (Box::point_type lower_, Box::point_type upper_)
        : lower(detail::lower_limits(lower_, upper_)),
          upper(detail::upper_limits(lower_, upper_))
    {
    }

    template<class Repr_, std::size_t Size_>
    constexpr bool Box<Repr_, Size_>::operator == (const Box& other) const
    {
        return lower == other.lower && upper == other.upper;
    }

    template<class Repr_, std::size_t Size_>
    constexpr bool Box<Repr_, Size_>::operator != (const Box& other) const
    {
        return !operator ==(other);
    }
}

namespace geom
{
    template<class Repr_, std::size_t Size_>
    constexpr bool
    intersects (const Box<Repr_, Size_>& lhs, const Box<Repr_, Size_>& rhs)
    {
        for (std::size_t dim = 0; dim < Size_; ++dim)
        {
            const bool gap_exists = lhs.upper[dim] < rhs.lower[dim] ||
                                    rhs.upper[dim] < lhs.lower[dim];

            if (gap_exists) return false;
        }
        return true;
    }

    namespace detail
    {
        /* Doesn't actually require a call because everything is constexpr but who cares */
        void static_box_tests ()
        {
            constexpr Point<int, 3> p1 = { 0, 1, 2 };
            constexpr Point<int, 3> p2 = { 1, 2, 3 };
            constexpr Point<int, 3> p3 = { 2, 3, 4 };
            constexpr Point<int, 3> p4 = { 3, 4, 5 };

            constexpr Box<int, 3> box12(p1, p2);
            constexpr Box<int, 3> box13(p1, p3);
            constexpr Box<int, 3> box14(p1, p4);
            constexpr Box<int, 3> box23(p2, p3);
            constexpr Box<int, 3> box24(p2, p4);
            constexpr Box<int, 3> box34(p3, p4);

            constexpr Box<int, 3> box21(p2, p1);
            constexpr Box<int, 3> box31(p3, p1);
            constexpr Box<int, 3> box41(p4, p1);
            constexpr Box<int, 3> box32(p3, p2);
            constexpr Box<int, 3> box42(p4, p2);
            constexpr Box<int, 3> box43(p4, p3);

            static_assert(box12 == box21, "Test ring-like construction");
            static_assert(box13 == box31, "Test ring-like construction");
            static_assert(box14 == box41, "Test ring-like construction");
            static_assert(box23 == box32, "Test ring-like construction");
            static_assert(box24 == box42, "Test ring-like construction");
            static_assert(box34 == box43, "Test ring-like construction");

            static_assert(!intersects(box12, box34), "Test no intersect");
            static_assert(!intersects(box34, box12), "Test no intersect");

            static_assert(intersects(box12, box23), "Test common point");
            static_assert(intersects(box23, box34), "Test common point");

            static_assert(intersects(box13, box24), "Test common region");
            static_assert(intersects(box42, box13), "Test common region");

            static_assert(intersects(box12, box13), "Test included region");
            static_assert(intersects(box12, box14), "Test included region");
            static_assert(intersects(box13, box14), "Test included region");
            static_assert(intersects(box23, box14), "Test included region");

            static_assert(intersects(box12, box12), "Test self-intersection");
            static_assert(intersects(box13, box13), "Test self-intersection");
            static_assert(intersects(box14, box14), "Test self-intersection");
            static_assert(intersects(box23, box23), "Test self-intersection");
            static_assert(intersects(box24, box24), "Test self-intersection");
            static_assert(intersects(box34, box34), "Test self-intersection");
        }
    }
}


#endif //EDA_BOX_HPP
