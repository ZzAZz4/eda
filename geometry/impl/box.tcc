#ifndef EDA_BOX_TCC
#define EDA_BOX_TCC

namespace geom {
    /* Currently unused. */
    /* May remove if ends up being completely unnecessary to define orderings later */
    /* Implementation details */
    namespace detail {
        /* Returns a point left-enclosing a region defined by two points */
        template<class Repr_, std::size_t Size_>
        constexpr Point <Repr_, Size_> lower_limits (
            const Point <Repr_, Size_>& lhs,
            const Point <Repr_, Size_>& rhs) {
            Point <Repr_, Size_> result;
            for (std::size_t i = 0; i < Size_; ++i)
                result[i] = std::min(lhs[i], rhs[i]);

            return result;
        }

        /* Returns a point right-enclosing a region defined by two points */
        template<class Repr_, std::size_t Size_>
        constexpr Point <Repr_, Size_> upper_limits (
            const Point <Repr_, Size_>& lhs,
            const Point <Repr_, Size_>& rhs) {
            Point <Repr_, Size_> result;
            for (std::size_t i = 0; i < Size_; ++i)
                result[i] = std::max(lhs[i], rhs[i]);

            return result;
        }
    }
}
namespace geom {
    template<class Point_>
    constexpr
    Box<Point_>::Box (Box::point_type lower_, Box::point_type upper_)
        : lower(lower_), upper(upper_) {
        // Nothing to do here
    }

    template<class Point_>
    constexpr bool Box<Point_>::operator == (const Box& other) const {
        return lower == other.lower && upper == other.upper;
    }

    template<class Point_>
    constexpr bool Box<Point_>::operator != (const Box& other) const {
        return !operator ==(other);
    }


    namespace tests {
#ifdef GEOMETRY_COMPILE_TESTS
        /* Ensure that a box can be constructed */
        void box_construct_tests () {
            using point_type = Point<int, 3>;

            constexpr point_type prr = { +1, +1 };
            constexpr point_type prl = { +1, -1 };
            constexpr point_type plr = { -1, +1 };
            constexpr point_type pll = { -1, -1 };

            constexpr Box box1(pll, prr);
            constexpr Box box2(plr, prl);
            constexpr Box box3(prr, pll);
            constexpr Box box4(prl, plr);
        }
#endif
    }
}

#endif //EDA_BOX_TCC
