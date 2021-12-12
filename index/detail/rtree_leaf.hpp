#ifndef EDA_RTREE_LEAF_HPP
#define EDA_RTREE_LEAF_HPP

#include <cstddef>
#include "rtree_base.hpp"

namespace index_::detail {
    template<class Record_, class Box_, std::size_t M_, std::size_t m_>
    struct RTreeLeaf : public RTreeBase<Record_, Box_, M_, m_> {
        using base_type = RTreeBase<Record_, Box_, M_, m_>;
        using leaf_type = typename base_type::leaf_type;
        using inner_type = typename base_type::inner_type;

        using box_type = typename base_type::box_type;
        using point_type = typename base_type::point_type;
        using size_type = typename base_type::size_type;
        using box_storage = typename base_type::box_storage;
        using base_type::capacity;
        using base_type::lower_cnt;

        using record_type = Record_;
        using record_storage = record_type;
        using split_type = typename base_type::split_type;

        using base_type::boxes;
        record_storage records[capacity];
        using base_type::size;

        RTreeLeaf ();

        ~RTreeLeaf () = default;

        std::optional<split_type>
        _insert_helper (const box_type& box, const Record_& record);

        template<class OutputIter>
        OutputIter
        _query_helper (const box_type& box, OutputIter out) const;
    };
}

#include "impl/rtree_leaf_impl.hpp"


#endif //EDA_RTREE_LEAF_HPP
