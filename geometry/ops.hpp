#ifndef EDA_OPS_HPP
#define EDA_OPS_HPP

#include "box.hpp"
#include <optional>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace geom {
    template<class Point_>
    constexpr auto
    area (const Box <Point_>& box) {
        typename Box<Point_>::value_type result = 1;
        for (std::size_t i = 0; i < Point_::size(); ++i) {
            result *= abs(box.upper[i] - box.lower[i]);
        }
        return result;
    }

    template<class Point_>
    constexpr bool
    intersects (const Box <Point_>& lhs, const Box <Point_>& rhs) {
        for (std::size_t dim = 0; dim < Point_::size(); ++dim) {
            const bool gap_exists = lhs.upper[dim] < rhs.lower[dim] ||
                                    rhs.upper[dim] < lhs.lower[dim];

            if (gap_exists) { return false; }
        }
        return true;
    }

    template<class Point_>
    constexpr std::optional<Box < Point_>>

    intersection (const Box <Point_>& lhs, const Box <Point_>& rhs) {
        if (!intersects(lhs, rhs)) {
            return std::nullopt;
        }

        Point_ lower{};
        Point_ upper{};

        for (std::size_t dim = 0; dim < Point_::size(); ++dim) {
            lower[dim] = std::max(lhs.lower[dim], rhs.lower[dim]);
        }

        for (std::size_t dim = 0; dim < Point_::size(); ++dim) {
            upper[dim] = std::min(lhs.upper[dim], rhs.upper[dim]);
        }

        return Box<Point_>(lower, upper);
    }

    template<class Point_>
    constexpr Box <Point_>
    join (const Box <Point_>& lhs, const Box <Point_>& rhs) {
        Point_ lower{};
        Point_ upper{};

        for (std::size_t dim = 0; dim < Point_::size(); ++dim) {
            lower[dim] = std::min(lhs.lower[dim], rhs.lower[dim]);
        }

        for (std::size_t dim = 0; dim < Point_::size(); ++dim) {
            upper[dim] = std::max(lhs.upper[dim], rhs.upper[dim]);
        }

        return Box<Point_>(lower, upper);
    }

    template<class Point_>
    constexpr typename Box<Point_>::value_type
    join_enlargement (const Box <Point_>& base, const Box <Point_>& ext) {
        typename Box<Point_>::value_type cost{};

        for (std::size_t dim = 0; dim < Point_::size(); ++dim) {
            cost = cost +
                   std::max((base.lower[dim] - base.lower[dim]), base.lower[dim] - ext.lower[dim]);
        }

        for (std::size_t dim = 0; dim < Point_::size(); ++dim) {
            cost = cost +
                   std::max((ext.upper[dim] - ext.upper[dim]), ext.upper[dim] - base.upper[dim]);
        }

        return cost;
    }


    template<class Repr_, std::size_t Size_, class Res_ = double>
    constexpr Res_
    sq_distance (
        const Point <Repr_, Size_>& lhs, const Point <Repr_, Size_>& rhs) {
        Res_ result{};
        for (size_t i = 0; i < Size_; ++i) {
            result = result + (lhs[i] - rhs[i]) * (lhs[i] - rhs[i]);
        }
        return result;
    }

    template<class Repr_, std::size_t Size_, class Res_ = double>
    constexpr Res_
    distance (
        const Point <Repr_, Size_>& lhs, const Point <Repr_, Size_>& rhs) {
        return std::sqrt(sq_distance(lhs, rhs));
    }

    template<class Point_, class Res_ = double>
    constexpr Res_
    min_sq_distance (const Box <Point_>& lhs, const Box <Point_>& rhs) {
        using value = typename Point_::value_type;
        Point_ temporary{};

        for (std::size_t dim = 0; dim < Point_::size(); ++dim) {
            const bool gap_exists = lhs.upper[dim] < rhs.lower[dim] ||
                                    rhs.upper[dim] < lhs.lower[dim];

            if (!gap_exists) { continue; }

            temporary[dim] = std::max(
                std::max(value{}, rhs.lower[dim] - lhs.upper[dim]),
                std::max(value{}, lhs.lower[dim] - rhs.upper[dim]));
        }
        return sq_distance(temporary, Point_{});
    }

    namespace tests {
#ifdef GEOMETRY_COMPILE_TESTS
        void box_intersection_tests () {
            constexpr Point<int, 3> p1 = { 0, 1, 2 };
            constexpr Point<int, 3> p2 = { 1, 2, 3 };
            constexpr Point<int, 3> p3 = { 2, 3, 4 };
            constexpr Point<int, 3> p4 = { 3, 4, 5 };

            constexpr Box b12(p1, p2);
            constexpr Box b13(p1, p3);
            constexpr Box b14(p1, p4);
            constexpr Box b23(p2, p3);
            constexpr Box b24(p2, p4);
            constexpr Box b34(p3, p4);

            constexpr Box b11(p1, p1);
            constexpr Box b22(p2, p2);
            constexpr Box b33(p3, p3);
            constexpr Box b44(p4, p4);

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

        void box_join_tests () {
            constexpr Point<int, 3> p1 = { 0, 1, 2 };
            constexpr Point<int, 3> p2 = { 1, 2, 3 };
            constexpr Point<int, 3> p3 = { 2, 3, 4 };
            constexpr Point<int, 3> p4 = { 3, 4, 5 };

            constexpr Box b12(p1, p2);
            constexpr Box b13(p1, p3);
            constexpr Box b14(p1, p4);
            constexpr Box b23(p2, p3);
            constexpr Box b24(p2, p4);
            constexpr Box b34(p3, p4);

            constexpr Box b11(p1, p1);
            constexpr Box b22(p2, p2);
            constexpr Box b33(p3, p3);
            constexpr Box b44(p4, p4);

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
#endif
    }
}

#endif //EDA_OPS_HPP
