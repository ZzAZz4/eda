#ifndef EDA_RTREE_BASE_IMPL_HPP
#define EDA_RTREE_BASE_IMPL_HPP

#include <index/detail/rtree_base.hpp>
#include <optional>

#define TEMPLATE template<class Record_, class Box_, std::size_t M_, std::size_t m_>
#define NODE RTreeBase<Record_, Box_, M_, m_>

namespace index_::detail {
    TEMPLATE
    std::optional<typename NODE::split_type>
    NODE::_insert_helper_dispatch (const box_type& box, const Record_& record) {
        if (this->is_leaf) {
            return static_cast<leaf_type*>(this)->_insert_helper(box, record);
        }
        return static_cast<inner_type*>(this)->_insert_helper(box, record);
    }

    TEMPLATE
    template<class OutputIter>
    OutputIter
    NODE::_query_helper_dispatch (
        const box_type& box, OutputIter out) const {
        if (this->is_leaf) {
            return reinterpret_cast<const leaf_type*>(this)->_query_helper(box, out);
        }
        return reinterpret_cast<const inner_type*>(this)->_query_helper(box, out);
    }
}

#undef TEMPLATE
#undef NODE

#endif //EDA_RTREE_BASE_IMPL_HPP
