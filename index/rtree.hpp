#ifndef EDA_RTREE_HPP
#define EDA_RTREE_HPP

#include "rtree_node.hpp"
#include "../geometry/ops.hpp"

namespace index
{
    struct not_implemented : std::exception
    {
        [[nodiscard]] const char*
        what () const noexcept override { return "Not implemented!\n"; }
    };

    template<class Record_, class Repr_, std::size_t Dims_, std::size_t M_, std::size_t m_>
    struct RTree
    {
    public:
        using node_type = detail::RTreeBase<Record_, Repr_, Dims_, M_, m_>;
        using leaf_node = detail::RTreeLeaf<Record_, Repr_, Dims_, M_, m_>;
        using inner_node = detail::RTreeInner<Record_, Repr_, Dims_, M_, m_>;

        using node_pointer = node_type*;
        using leaf_pointer = leaf_node*;
        using inner_pointer = inner_node*;
        using const_node_pointer = const node_type*;
        using const_leaf_pointer = const leaf_node*;
        using const_inner_pointer = const inner_node*;
        using box_type = typename node_type::box_type;
        using point_type = typename node_type::point_type;
        using record_type = Record_;

    public:
        node_pointer root = nullptr;

        ~RTree () { delete root; }

        bool insert (const box_type& box, const record_type& record)
        {
            if (!root)
            {
                auto node = new leaf_node;
                root = node;
                return try_construct_at(node, box, record);
            }
            return insert_helper(root, box, record);
            throw not_implemented();
            return false;
        }

        auto insert_helper (
            node_pointer base,
            const box_type& box, const record_type& record)
        {
            if (base->_c_is_leaf) {
                auto node = static_cast<leaf_pointer>(base);
                auto trial = try_construct_at(node, box, record);

                /// TODO: handle full leaves
                if (!trial) throw not_implemented();

                return true;
            }
            auto node = static_cast<inner_pointer>(base);
            /// TODO: handle insert in inner nodes
            throw not_implemented();

        }


        bool try_construct_at (
            leaf_pointer node,
            const box_type& box, const record_type& record)
        {
            if (node->size == node->capacity) return false;
            new(&node->boxes[node->size]) box_type(box);
            new(&node->records[node->size]) typename leaf_node::record_type(
                record);
            node->size++;
            return true;
        }

        bool try_construct_at (
            inner_pointer node,
            const box_type& box, node_pointer record)
        {
            if (node->size == node->capacity) return false;
            new(&node->boxes[node->size]) box_type(box);
            new(&node->records[node->size]) typename inner_node::record_type(
                record);
            node->size++;
            return true;
        }
    };


}

#endif //EDA_RTREE_HPP
