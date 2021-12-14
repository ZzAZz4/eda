#ifndef EDA_RTREE_BASE_IMPL_HPP
#define EDA_RTREE_BASE_IMPL_HPP

#include "rtree_base.hpp"
#include <optional>

#define TEMPLATE template<class Record_, class Box_, std::size_t M_, std::size_t m_>
#define NODE RTreeBase<Record_, Box_, M_, m_>

namespace index_::detail {
    TEMPLATE
    std::optional<typename NODE::split_type>
    NODE::_insert_helper_dispatch (const box_type& box, const Record_& record) {
        switch (this->type) {
        case RTreeTag::LEAF: return static_cast<leaf_type*>(this)->_insert_helper(box, record);
        case RTreeTag::INNER: return static_cast<inner_type*>(this)->_insert_helper(box, record);
        default: std::abort();
        }
    }

    TEMPLATE
    template<class OutputIter>
    OutputIter
    NODE::_query_helper_dispatch (const box_type& box, OutputIter out) const {
        switch (this->type) {
        case RTreeTag::LEAF: return static_cast<const leaf_type*>(this)->_query_helper(box, out);
        case RTreeTag::INNER: return static_cast<const inner_type*>(this)->_query_helper(box, out);
        default: std::abort();
        }
    }
}

#undef TEMPLATE
#undef NODE

#endif //EDA_RTREE_BASE_IMPL_HPP
