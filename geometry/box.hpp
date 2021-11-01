#ifndef EDA_BOX_HPP
#define EDA_BOX_HPP

#include "point.hpp"

template <class Repr_, std::size_t Size_>
class Box
{
    using point_type = Point<Repr_, Size_>;
    using size_type = typename point_type::size_type;
    using value_type = typename point_type::value_type;

    point_type m_lower_bound;
    point_type m_upper_bound;

    constexpr Box(point_type lower, point_type upper) : m_lower_bound(lower), m_upper_bound(upper){}
};

#endif //EDA_BOX_HPP
