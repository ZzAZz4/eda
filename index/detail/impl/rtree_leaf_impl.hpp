//
// Created by Esteban on 12/11/2021.
//

#ifndef EDA_RTREE_LEAF_IMPL_HPP
#define EDA_RTREE_LEAF_IMPL_HPP

#include <index/detail/rtree_leaf.hpp>
#include "helpers.hpp"
#include <optional>

#define TEMPLATE template<class Record_, class Box_, std::size_t M_, std::size_t m_>
#define NODE RTreeLeaf<Record_, Box_, M_, m_>

namespace index_::detail {
    TEMPLATE NODE::
    RTreeLeaf () : base_type(true) {}

    TEMPLATE
    std::optional<typename NODE::split_type>
    NODE::_insert_helper (const box_type& box, const Record_& record) {
        auto inserted = detail::_try_add<leaf_type>(this, box, record);
        if (inserted) { // no split required
            return std::nullopt;
        }
        // create split and propagate to caller
        return _handle_split_insert<leaf_type>(this, box, record);
    }

    TEMPLATE
    template<class OutputIter>
    OutputIter
    NODE::_query_helper (const box_type& box, OutputIter out) const {
        for (size_t i = 0; i < this->size; ++i) {
            if (geom::intersects(box, this->boxes[i])) {
                *out++ = this->records[i];
            }
        }
        return out;
    }
}
#undef TEMPLATE
#undef NODE
#endif //EDA_RTREE_LEAF_IMPL_HPP
