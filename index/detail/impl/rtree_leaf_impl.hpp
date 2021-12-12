//
// Created by Esteban on 12/11/2021.
//

#ifndef EDA_RTREE_LEAF_IMPL_HPP
#define EDA_RTREE_LEAF_IMPL_HPP

#include <index/detail/rtree_leaf.hpp>
#include "helpers.hpp"
#include <optional>

namespace index_::detail {
    template<class Record_, class Box_, std::size_t M_, std::size_t m_>
    RTreeLeaf<Record_, Box_, M_, m_>::RTreeLeaf () : base_type(true) {}

    template<class Record_, class Box_, std::size_t M_, std::size_t m_>
    std::optional<typename RTreeLeaf<Record_, Box_, M_, m_>::split_type>
    RTreeLeaf<Record_, Box_, M_, m_>::_insert_helper (const box_type& box, const Record_& record) {
        auto inserted = detail::_try_add<leaf_type>(this, box, record);
        if (inserted) { // no split required
            return std::nullopt;
        }
        // create split and propagate to caller
        return _handle_split_insert<leaf_type>(this, box, record);
    }

    template<class Record_, class Box_, std::size_t M_, std::size_t m_>
    template<class OutputIter>
    OutputIter
    RTreeLeaf<Record_, Box_, M_, m_>::_query_helper (const box_type& box, OutputIter out) const {
        for (size_t i = 0; i < this->size; ++i) {
            if (geom::intersects(box, this->boxes[i])) {
                *out++ = this->records[i];
            }
        }
        return out;
    }
}

#endif //EDA_RTREE_LEAF_IMPL_HPP
