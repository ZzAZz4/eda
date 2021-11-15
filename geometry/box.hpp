#ifndef EDA_BOX_HPP
#define EDA_BOX_HPP

#include "point.hpp"

namespace geom
{
    template<class Repr_, std::size_t Size_>
    struct Box
    {
        using point_type = Point<Repr_, Size_>;
        using size_type [[maybe_unused]] = typename point_type::size_type;
        using value_type [[maybe_unused]] = typename point_type::value_type;

        /* const so that nobody does something stupid with the ring */
        /* lower point on the box on all dimensions */
        point_type lower;

        /* upper point on the box on all dimensions */
        point_type upper;

        /* Constructor.
         * Creates a bbox that encloses both lower and upper,
         * regardless of order on each dimension */
        constexpr Box (point_type lower_, point_type upper_);

        /* Comparison */
        constexpr bool operator == (const Box& other) const;

        /* Comparison */
        constexpr bool operator != (const Box& other) const;
    };
}

#include "impl/box.tcc"


#endif //EDA_BOX_HPP
