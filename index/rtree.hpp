#ifndef EDA_RTREE_HPP
#define EDA_RTREE_HPP

#include <iostream>
#include <thread>
#include <functional>
#include <geometry/ops.hpp>
#include "rtree_nodes.hpp"

namespace index_ {
    template<class Record_, class Box_, std::size_t M_, std::size_t m_ = (M_ + 1) / 2>
    struct RTree {
    public:
        using base_node = detail::RTreeBase<Record_, Box_, M_, m_>;
        using leaf_node = typename base_node::leaf_type;
        using inner_node = typename base_node::inner_type;

        using box_type = typename base_node::box_type;
        using point_type = typename base_node::point_type;
        using record_type = Record_;

        constexpr static auto capacity = M_;

    public:
        constexpr static auto max_left_rebuild_size = m_;
        constexpr static auto max_right_rebuild_size = M_ + 1 - m_;
        static_assert((M_ + 1) == max_left_rebuild_size + max_right_rebuild_size);

        base_node* root = nullptr;
    public:
        ~RTree () { delete root; }

        /* Inserts a box-record pair in the tree
         * Returns true if successful (it should always be successful tho...) */
        bool
        insert (const box_type& box, const record_type& record) {
            if (!root) {
                root = new leaf_node;
                detail::_try_add<leaf_node>(root, box, record);
                return true;
            }
            // call recursive function on the root. This may propagate a split
            auto subtree_prop = root->_insert_helper_dispatch(box, record);
            if (!subtree_prop.has_value()) {
                return true;
            }
            // split the root
            root = new inner_node;
            detail::_try_add<inner_node>(root, subtree_prop->right.mbr, subtree_prop->right.node);
            detail::_try_add<inner_node>(root, subtree_prop->left.mbr, subtree_prop->left.node);
            return true;
        }

        /* Gets all the records that intersect a query box in the tree.
         * Stores the result into the range starting at out.
         * Returns the end of the new range.
         * Usage:
         * >> std::vector<T> results;
         * >> tree.query(box, std::back_inserter(results));
         */
        template<class OutputIter>
        OutputIter
        query (const box_type& box, OutputIter out) const {
            if (this->root == nullptr) {
                return out;
            }
            return root->_query_helper_dispatch(box, out);
        }
    };
}

#endif //EDA_RTREE_HPP
