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

    template<class Repr_, std::size_t Size_>
    constexpr Box<Repr_, Size_>
    join (const Box<Repr_, Size_>& lhs, const Box<Repr_, Size_>& rhs)
    {
        Point<Repr_, Size_> lower;
        Point<Repr_, Size_> upper;

        for (std::size_t dim = 0; dim < Size_; ++dim)
            lower[dim] = std::min(lhs.lower[dim], rhs.lower[dim]);

        for (std::size_t dim = 0; dim < Size_; ++dim)
            upper[dim] = std::max(lhs.upper[dim], rhs.upper[dim]);

        return Box<Repr_, Size_>(lower, upper);
    }


    namespace tests
    {
        void box_intersection_tests ()
        {
            constexpr Point<int, 3> p1 = { 0, 1, 2 };
            constexpr Point<int, 3> p2 = { 1, 2, 3 };
            constexpr Point<int, 3> p3 = { 2, 3, 4 };
            constexpr Point<int, 3> p4 = { 3, 4, 5 };

            constexpr Box<int, 3> b12(p1, p2);
            constexpr Box<int, 3> b13(p1, p3);
            constexpr Box<int, 3> b14(p1, p4);
            constexpr Box<int, 3> b23(p2, p3);
            constexpr Box<int, 3> b24(p2, p4);
            constexpr Box<int, 3> b34(p3, p4);

            constexpr Box<int, 3> b11(p1, p1);
            constexpr Box<int, 3> b22(p2, p2);
            constexpr Box<int, 3> b33(p3, p3);
            constexpr Box<int, 3> b44(p4, p4);

            static_assert(!intersects(b12, b34), "Test no intersect");
            static_assert(!intersects(b34, b12), "Test no intersect");
            static_assert(!intersection(b12, b34), "Test no intersect");
            static_assert(!intersection(b34, b12), "Test no intersect");

            static_assert(intersects(b12, b23), "Test common point");
            static_assert(intersects(b23, b34), "Test common point");
            static_assert(*intersection(b23, b34) == b33, "Test common point");
            static_assert(*intersection(b12, b23) == b22, "Test common point");

            static_assert(intersects(b13, b24), "Test common region");
            static_assert(intersects(b24, b13), "Test common region");
            static_assert(*intersection(b13, b24) == b23, "Test common region");
            static_assert(*intersection(b24, b13) == b23, "Test common region");

            static_assert(intersects(b12, b13), "Test lhs region");
            static_assert(intersects(b12, b14), "Test lhs region");
            static_assert(intersects(b13, b14), "Test lhs region");
            static_assert(intersects(b23, b14), "Test lhs region");
            static_assert(*intersection(b12, b13) == b12, "Test lhs region");
            static_assert(*intersection(b12, b14) == b12, "Test lhs region");
            static_assert(*intersection(b13, b14) == b13, "Test lhs region");
            static_assert(*intersection(b23, b14) == b23, "Test lhs region");

            static_assert(intersects(b12, b12), "Test self");
            static_assert(intersects(b13, b13), "Test self");
            static_assert(intersects(b14, b14), "Test self");
            static_assert(intersects(b23, b23), "Test self");
            static_assert(intersects(b24, b24), "Test self");
            static_assert(intersects(b34, b34), "Test self");
            static_assert(*intersection(b12, b12) == b12, "Test self");
            static_assert(*intersection(b13, b13) == b13, "Test self");
            static_assert(*intersection(b14, b14) == b14, "Test self");
            static_assert(*intersection(b23, b23) == b23, "Test self");
            static_assert(*intersection(b24, b24) == b24, "Test self");
            static_assert(*intersection(b34, b34) == b34, "Test self");
        }

        void box_join_tests ()
        {
            constexpr Point<int, 3> p1 = { 0, 1, 2 };
            constexpr Point<int, 3> p2 = { 1, 2, 3 };
            constexpr Point<int, 3> p3 = { 2, 3, 4 };
            constexpr Point<int, 3> p4 = { 3, 4, 5 };

            constexpr Box<int, 3> b12(p1, p2);
            constexpr Box<int, 3> b13(p1, p3);
            constexpr Box<int, 3> b14(p1, p4);
            constexpr Box<int, 3> b23(p2, p3);
            constexpr Box<int, 3> b24(p2, p4);
            constexpr Box<int, 3> b34(p3, p4);

            constexpr Box<int, 3> b11(p1, p1);
            constexpr Box<int, 3> b22(p2, p2);
            constexpr Box<int, 3> b33(p3, p3);
            constexpr Box<int, 3> b44(p4, p4);

            static_assert(join(b11, b22) == b12, "Join points");
            static_assert(join(b11, b33) == b13, "Join points");
            static_assert(join(b11, b44) == b14, "Join points");
            static_assert(join(b22, b33) == b23, "Join points");
            static_assert(join(b22, b44) == b24, "Join points");
            static_assert(join(b33, b44) == b34, "Join points");

            static_assert(join(b12, b34) == b14, "Join separate");
            static_assert(join(b34, b12) == b14, "Join separate");

            static_assert(join(b13, b24) == b14, "Join intersected");
            static_assert(join(b24, b13) == b14, "Join intersected");

            static_assert(join(b14, b23) == b14, "Join included");
            static_assert(join(b14, b12) == b14, "Join included");
            static_assert(join(b13, b12) == b13, "Join included");
        }
    }
}

#endif //EDA_OPS_HPP
