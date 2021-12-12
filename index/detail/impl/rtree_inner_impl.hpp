#ifndef EDA_RTREE_INNER_IMPL_HPP
#define EDA_RTREE_INNER_IMPL_HPP

#include <index/detail/rtree_inner.hpp>
#include <optional>
#include "helpers.hpp"

#define TEMPLATE template<class Record_, class Box_, std::size_t M_, std::size_t m_>
#define NODE RTreeInner<Record_, Box_, M_, m_>

namespace index_::detail {
    TEMPLATE NODE::
    RTreeInner () : base_type(false) {}

    TEMPLATE NODE::
    ~RTreeInner () {
        for (size_t i = 0; i < this->size; ++i) {
            delete this->records[i];
        }
    }

    TEMPLATE
    std::optional<typename NODE::split_type>
    NODE::_insert_helper (const box_type& box, const Record_& record) {
        size_t idx = detail::_min_enlargement_index(this->boxes, this->size, box);

        // _insert_helper_dispatch in that entry. This may propagate a split.
        auto subtree_prop = records[idx]->_insert_helper_dispatch(box, record);

        if (!subtree_prop.has_value()) { // no split to propagate.
            // adjust the mbr
            boxes[idx] = geom::join(boxes[idx], box);
            return std::nullopt;
        }
        // split
        auto split = *subtree_prop;
        boxes[idx] = split.left.mbr;
        records[idx] = split.left.node;

        auto success = detail::_try_add<inner_type>(this, split.right.mbr, split.right.node);
        if (success) { // no split required
            return std::nullopt;
        }
        // create split and propagate to caller
        return _handle_split_insert<inner_type>(this, split.right.mbr, split.right.node);

    }

    TEMPLATE
    template<class OutIt>
    OutIt
    NODE::_query_helper (const box_type& box, OutIt out) const {
        for (size_t i = 0; i < this->size; ++i) {
            if (geom::intersects(box, this->boxes[i])) {
                out = this->records[i]->_query_helper_dispatch(box, out);
            }
        }
        return out;
    }
}

#undef TEMPLATE
#undef NODE


#endif //EDA_RTREE_INNER_IMPL_HPP
