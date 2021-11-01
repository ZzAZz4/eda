#ifndef EDA_OPS_HPP
#define EDA_OPS_HPP

#include "box.hpp"

namespace geom
{
    template<class Repr_, std::size_t Size_>
    constexpr bool
    intersects (const Box <Repr_, Size_>& lhs, const Box <Repr_, Size_>& rhs)
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
        void box_intersect_tests ()
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

            static_assert(!intersects(box12, box34), "Test no intersect");
            static_assert(!intersects(box34, box12), "Test no intersect");

            static_assert(intersects(box12, box23), "Test common point");
            static_assert(intersects(box23, box34), "Test common point");

            static_assert(intersects(box13, box24), "Test common region");
            static_assert(intersects(box24, box13), "Test common region");

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

#endif //EDA_OPS_HPP
