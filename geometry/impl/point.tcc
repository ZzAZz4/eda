#ifndef EDA_POINT_TCC
#define EDA_POINT_TCC

namespace geom
{
    template<class Repr_, std::size_t Size_>
    constexpr typename Point<Repr_, Size_>::size_type
    Point<Repr_, Size_>::size ()
    {
        return Size_;
    }

    template<class Repr_, std::size_t Size_>
    constexpr bool
    Point<Repr_, Size_>::operator == (const Point& other) const
    {
        for (size_type i = 0; i < size(); ++i)
            if (!(this->_m_buff[i] == other._m_buff[i]))
                return false;

        return true;
    }

    template<class Repr_, std::size_t Size_>
    constexpr bool Point<Repr_, Size_>::operator != (const Point& other) const
    {
        return !operator ==(other);
    }

    template<class Repr_, std::size_t Size_>
    constexpr auto
    Point<Repr_, Size_>::begin () const
    {
        return std::begin(this->_m_buff);
    }

    template<class Repr_, std::size_t Size_>
    constexpr auto
    Point<Repr_, Size_>::end () const
    {
        return std::end(this->_m_buff);
    }

    template<class Repr_, std::size_t Size_>
    constexpr auto Point<Repr_, Size_>::begin ()
    {
        return std::begin(this->_m_buff);
    }

    template<class Repr_, std::size_t Size_>
    constexpr auto Point<Repr_, Size_>::end ()
    {
        return std::end(this->_m_buff);
    }

    template<class Repr_, std::size_t Size_>
    constexpr const typename Point<Repr_, Size_>::value_type&
    Point<Repr_, Size_>::operator [] (Point::size_type dim) const
    {
        return this->_m_buff[dim];
    }

    template<class Repr_, std::size_t Size_>
    constexpr typename Point<Repr_, Size_>::value_type&
    Point<Repr_, Size_>::operator [] (Point::size_type dim)
    {
        return this->_m_buff[dim];
    }
}

#endif //EDA_POINT_TCC
