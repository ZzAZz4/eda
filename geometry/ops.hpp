#ifndef EDA_OPS_HPP
#define EDA_OPS_HPP

#include "box.hpp"
#include <optional>

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

    template<class Repr_, std::size_t Size_>
    constexpr std::optional<Box<Repr_, Size_>>
    intersection (const Box<Repr_, Size_>& lhs, const Box<Repr_, Size_>& rhs)
    {
        if (!intersects(lhs, rhs))
            return std::nullopt;

        Point<Repr_, Size_> lower;
        Point<Repr_, Size_> upper;

        for (std::size_t dim = 0; dim < Size_; ++dim)
            lower[dim] = std::max(lhs.lower[dim], rhs.lower[dim]);

        for (std::size_t dim = 0; dim < Size_; ++dim)
            upper[dim] = std::min(lhs.upper[dim], rhs.upper[dim]);

        return Box<Repr_, Size_>(lower, upper);
    }


    namespace detail
    {
        void box_intersection_tests ()
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

            constexpr Box<int, 3> box11(p1, p1);
            constexpr Box<int, 3> box22(p2, p2);
            constexpr Box<int, 3> box33(p3, p3);
            constexpr Box<int, 3> box44(p4, p4);

            static_assert(!intersects(box12, box34), "Test no intersect");
            static_assert(!intersects(box34, box12), "Test no intersect");
            static_assert(
                !intersection(box12, box34).has_value(), "Test no intersect");
            static_assert(
                !intersection(box34, box12).has_value(), "Test no intersect");

            static_assert(intersects(box12, box23), "Test common point");
            static_assert(intersects(box23, box34), "Test common point");
            static_assert(
                *intersection(box23, box34) == box33, "Test common point");
            static_assert(
                *intersection(box12, box23) == box22, "Test common point");

            static_assert(intersects(box13, box24), "Test common region");
            static_assert(intersects(box24, box13), "Test common region");
            static_assert(
                *intersection(box13, box24) == box23, "Test common region");
            static_assert(
                *intersection(box24, box13) == box23, "Test common region");

            static_assert(intersects(box12, box13), "Test included region");
            static_assert(intersects(box12, box14), "Test included region");
            static_assert(intersects(box13, box14), "Test included region");
            static_assert(intersects(box23, box14), "Test included region");
            static_assert(
                *intersection(box12, box13) == box12, "Test included region");
            static_assert(
                *intersection(box12, box14) == box12, "Test included region");
            static_assert(
                *intersection(box13, box14) == box13, "Test included region");
            static_assert(
                *intersection(box23, box14) == box23, "Test included region");

            static_assert(intersects(box12, box12), "Test self-intersection");
            static_assert(intersects(box13, box13), "Test self-intersection");
            static_assert(intersects(box14, box14), "Test self-intersection");
            static_assert(intersects(box23, box23), "Test self-intersection");
            static_assert(intersects(box24, box24), "Test self-intersection");
            static_assert(intersects(box34, box34), "Test self-intersection");
            static_assert(
                *intersection(box12, box12) == box12, "Test self-intersection");
            static_assert(
                *intersection(box13, box13) == box13, "Test self-intersection");
            static_assert(
                *intersection(box14, box14) == box14, "Test self-intersection");
            static_assert(
                *intersection(box23, box23) == box23, "Test self-intersection");
            static_assert(
                *intersection(box24, box24) == box24, "Test self-intersection");
            static_assert(
                *intersection(box34, box34) == box34, "Test self-intersection");
        }
    }
}

#endif //EDA_OPS_HPP
