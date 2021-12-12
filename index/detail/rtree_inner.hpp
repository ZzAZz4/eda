#ifndef EDA_RTREE_INNER_HPP
#define EDA_RTREE_INNER_HPP

#include <cstddef>
#include "rtree_base.hpp"

namespace index_::detail {
    template<class Record_, class Box_, std::size_t M_, std::size_t m_>
    struct RTreeInner : public RTreeBase<Record_, Box_, M_, m_> {
    public:
        using base_type = RTreeBase<Record_, Box_, M_, m_>;
        using leaf_type = typename base_type::leaf_type;
        using inner_type = typename base_type::inner_type;

        using box_type = typename base_type::box_type;
        using point_type = typename base_type::point_type;
        using size_type = typename base_type::size_type;

        using base_type::capacity;
        using base_type::lower_cnt;
        using record_type = base_type*;

        using box_storage = typename base_type::box_storage;
        using split_type = typename base_type::split_type;

    public:
        RTreeInner ();

        ~RTreeInner ();

        std::optional<split_type>
        _insert_helper (const box_type& box, const Record_& record);

        template<class OutIt>
        OutIt _query_helper (const box_type& box, OutIt out) const;

    public:
        using base_type::boxes;
        record_type records[capacity];
        using base_type::size;

    };

}

#include "impl/rtree_inner_impl.hpp"

#endif //EDA_RTREE_INNER_HPP
