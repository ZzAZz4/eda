#ifndef EDA_BOX_HPP
#define EDA_BOX_HPP

#include "point.hpp"

namespace geom
{
    namespace detail
    {
        /* IMPLEMENTATION DETAIL:
         * Returns a point left-enclosing a region defined by two points */
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

        /* IMPLEMENTATION DETAIL:
         * Returns a point right-enclosing a region defined by two points */
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

        const point_type lower;
        const point_type upper;

        constexpr Box (point_type lower_, point_type upper_)
            : lower(detail::lower_limits(lower_, upper_)),
              upper(detail::upper_limits(lower_, upper_))
        {
        }

        constexpr bool operator == (const Box& other) const
        {
            return lower == other.lower && upper == other.upper;
        }

        constexpr bool operator != (const Box& other) const
        {
            return !operator == (other);
        }
    };

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

}
#endif //EDA_BOX_HPP
