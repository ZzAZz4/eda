#ifndef EDA_RTREE_BASE_IMPL_HPP
#define EDA_RTREE_BASE_IMPL_HPP

#include <index/detail/rtree_base.hpp>
#include <optional>

namespace index_::detail {
    template<class Record_, class Box_, std::size_t M_, std::size_t m_>
    std::optional<typename RTreeBase<Record_, Box_, M_, m_>::split_type>
    RTreeBase<Record_, Box_, M_, m_>::_insert_helper_dispatch (
        const box_type& box, const Record_& record) {
        if (this->is_leaf) {
            return static_cast<leaf_type*>(this)->_insert_helper(box, record);
        }
        return static_cast<inner_type*>(this)->_insert_helper(box, record);
    }

    template<class Record_, class Box_, std::size_t M_, std::size_t m_>
    template<class OutputIter>
    OutputIter RTreeBase<Record_, Box_, M_, m_>::_query_helper_dispatch (
        const box_type& box, OutputIter out) const {
        if (this->is_leaf) {
            return reinterpret_cast<const leaf_type*>(this)->_query_helper(box, out);
        }
        return reinterpret_cast<const inner_type*>(this)->_query_helper(box, out);
    }
}

#endif //EDA_RTREE_BASE_IMPL_HPP
