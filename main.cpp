#include <iostream>
#include "geometry/box.hpp"


int main ()
{
    constexpr geom::Point<int, 3> p1 = { 0, 1, 2 };
    constexpr geom::Point<int, 3> p2 = { 1, 2, 3 };
    constexpr geom::Point<int, 3> p3 = { 2, 3, 4 };
    constexpr geom::Point<int, 3> p4 = { 3, 4, 5 };

    constexpr geom::Box<int, 3> box12 (p1, p2);
    constexpr geom::Box<int, 3> box13 (p1, p3);
    constexpr geom::Box<int, 3> box14 (p1, p4);
    constexpr geom::Box<int, 3> box23 (p2, p3);
    constexpr geom::Box<int, 3> box24 (p2, p4);
    constexpr geom::Box<int, 3> box34 (p3, p4);

    /* Test ring-like behaviour */
    constexpr geom::Box<int, 3> box21 (p2, p1);
    constexpr geom::Box<int, 3> box31 (p3, p1);
    constexpr geom::Box<int, 3> box41 (p4, p1);
    constexpr geom::Box<int, 3> box32 (p3, p2);
    constexpr geom::Box<int, 3> box42 (p4, p2);
    constexpr geom::Box<int, 3> box43 (p4, p3);

    static_assert(box12 == box21);
    static_assert(box13 == box31);
    static_assert(box14 == box41);
    static_assert(box23 == box32);
    static_assert(box24 == box42);
    static_assert(box34 == box43);

    /* Test intersection */
    static_assert(!geom::intersects(box12, box34)); // no intersect
    static_assert(!geom::intersects(box34, box12)); // no intersect

    static_assert(geom::intersects(box12, box23)); // single point intersects
    static_assert(geom::intersects(box23, box34)); // single point intersects

    static_assert(geom::intersects(box13, box24)); // region intersects
    static_assert(geom::intersects(box42, box13)); // region intersects

    static_assert(geom::intersects(box12, box13)); // includes
    static_assert(geom::intersects(box12, box14)); // includes
    static_assert(geom::intersects(box13, box14)); // includes
    static_assert(geom::intersects(box23, box14)); // includes

    static_assert(geom::intersects(box12, box12)); // self-intersection
    static_assert(geom::intersects(box13, box13)); // self-intersection
    static_assert(geom::intersects(box14, box14)); // self-intersection
    static_assert(geom::intersects(box23, box23)); // self-intersection
    static_assert(geom::intersects(box24, box24)); // self-intersection
    static_assert(geom::intersects(box34, box34)); // self-intersection
}