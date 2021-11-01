#ifndef EDA_BOX_TCC
#define EDA_BOX_TCC

namespace geom
{
    /* Implementation details */
    namespace detail
    {
        /* Returns a point left-enclosing a region defined by two points */
        template<class Repr_, std::size_t Size_>
        constexpr Point <Repr_, Size_> lower_limits (
            const Point <Repr_, Size_>& lhs,
            const Point <Repr_, Size_>& rhs)
        {
            Point <Repr_, Size_> result;
            for (std::size_t i = 0; i < Size_; ++i)
                result[i] = std::min(lhs[i], rhs[i]);

            return result;
        }

        /* Returns a point right-enclosing a region defined by two points */
        template<class Repr_, std::size_t Size_>
        constexpr Point <Repr_, Size_> upper_limits (
            const Point <Repr_, Size_>& lhs,
            const Point <Repr_, Size_>& rhs)
        {
            Point <Repr_, Size_> result;
            for (std::size_t i = 0; i < Size_; ++i)
                result[i] = std::max(lhs[i], rhs[i]);

            return result;
        }
    }

    template<class Repr_, std::size_t Size_>
    constexpr
    Box<Repr_, Size_>::Box (Box::point_type lower_, Box::point_type upper_)
        : lower(detail::lower_limits(lower_, upper_)),
          upper(detail::upper_limits(lower_, upper_))
    {
        // Nothing to do here
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


    namespace detail
    {
        /* Ensure that a box is equal to its inverses */
        void box_construct_tests ()
        {
            constexpr Point<int, 3> prr = { 1, 1 };
            constexpr Point<int, 3> prl = { 1, -1 };
            constexpr Point<int, 3> plr = { -1, 1 };
            constexpr Point<int, 3> pll = { -1, -1 };

            constexpr Box<int, 3> box1(pll, prr);
            constexpr Box<int, 3> box2(plr, prl);
            constexpr Box<int, 3> box3(prr, pll);
            constexpr Box<int, 3> box4(prl, plr);

            static_assert(box1 == box2, "Test ring-like construction");
            static_assert(box2 == box3, "Test ring-like construction");
            static_assert(box3 == box4, "Test ring-like construction");
            static_assert(box4 == box1, "Test ring-like construction");
        }
    }
}

#endif //EDA_BOX_TCC
