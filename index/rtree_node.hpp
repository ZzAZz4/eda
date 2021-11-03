#ifndef EDA_RTREE_NODE_HPP
#define EDA_RTREE_NODE_HPP

#include <cstddef>
#include "../geometry/box.hpp"


namespace index::detail
{
    template<class Record_, class Repr_, std::size_t Dims_, std::size_t M_, std::size_t m_>
    struct RTreeBase
    {
        using base_type = RTreeBase;
        using box_type = ::geom::Box<Repr_, Dims_>;
        using point_type = typename box_type::point_type;
        using size_type = typename box_type::size_type;
        constexpr static std::size_t capacity = M_;
        constexpr static std::size_t lower_cnt = m_;

        using box_storage = typename std::aligned_storage<
            sizeof(box_type),
            alignof(box_type)
        >::type;

        // size
        // C-style array used on purpose (no default constructors for box)
        const bool _c_is_leaf;
        size_type size = 0;
        box_storage boxes[capacity];

        explicit RTreeBase (bool is_leaf) : _c_is_leaf(is_leaf) {}
    };

    template<class Record_, class Repr_, std::size_t Dims_, std::size_t M_, std::size_t m_>
    struct RTreeLeaf : public RTreeBase<Record_, Repr_, Dims_, M_, m_>
    {
        using base_type = RTreeBase<Record_, Repr_, Dims_, M_, m_>;
        using box_type = typename base_type::box_type;
        using point_type = typename base_type::point_type;
        using size_type = typename base_type::size_type;
        using box_storage = typename base_type::box_storage;
        constexpr static std::size_t capacity = base_type::capacity;
        constexpr static std::size_t lower_cnt = base_type::lower_cnt;

        using record_type = Record_;
        using record_storage = typename std::aligned_storage<
            sizeof(record_type),
            alignof(record_type)
        >::type;

        record_storage records[capacity];

        RTreeLeaf () : base_type(true) {}
    };

    template<class Record_, class Repr_, std::size_t Dims_, std::size_t M_, std::size_t m_>
    struct RTreeInner : public RTreeBase<Record_, Repr_, Dims_, M_, m_>
    {
        using base_type = RTreeBase<Record_, Repr_, Dims_, M_, m_>;
        using box_type = typename base_type::box_type;
        using point_type = typename base_type::point_type;
        using size_type = typename base_type::size_type;
        constexpr static std::size_t capacity = base_type::capacity;
        constexpr static std::size_t lower_cnt = base_type::lower_cnt;
        using record_type = base_type*;

        record_type records[capacity];

        RTreeInner () : base_type(false) {}
    };
}

#endif //EDA_RTREE_NODE_HPP
