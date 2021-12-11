#ifndef EDA_RTREE_HPP
#define EDA_RTREE_HPP

#include <iostream>
#include <thread>
#include <functional>
#include "rtree_node.hpp"
#include "../geometry/ops.hpp"

namespace index_ {
    namespace detail {
        template<size_t M_, class BoxPair>
        static size_t
        _farthest_from_source (size_t src, BoxPair (& recs)[M_]) {
            using geom::min_sq_distance;
            const auto& src_box = recs[src].first;

            auto it = std::max_element(
                recs, recs + M_, [&src_box] (const auto& a, const auto& b) {
                    return min_sq_distance(src_box, a.first) <
                           min_sq_distance(src_box, b.first);
                });

            auto res_idx = std::distance(recs, it);
            return res_idx;
        }

        template<class Box, class Node, class Record = typename Node::record_type>
        static bool
        _try_construct_at (Node* node, const Box& box, const Record& record) {
            if (node->size == node->capacity) {
                return false;
            }
            node->_boxes[node->size] = box;
            node->_records[node->size] = record;
            node->size++;
            return true;
        }

        template<class It, class Box>
        static size_t
        _min_enlargement_index (It begin, size_t size, const Box& box) {
            using geom::join_enlargement;
            auto it = std::min_element(
                begin, begin + size, [&box] (const auto& a, const auto& b) {
                    return join_enlargement(a, box) < join_enlargement(b, box);
                });
            auto index = std::distance(begin, it);
            return index;
        }
    }
}

namespace index_ {
    template<class Record_, class Box_, std::size_t M_, std::size_t m_ = (M_ + 1) / 2>
    struct RTree {
    public:
        using node_type = detail::RTreeBase<Record_, Box_, M_, m_>;
        using leaf_node = detail::RTreeLeaf<Record_, Box_, M_, m_>;
        using inner_node = detail::RTreeInner<Record_, Box_, M_, m_>;

        using node_pointer = node_type*;
        using leaf_pointer = leaf_node*;
        using inner_pointer = inner_node*;
        using const_node_pointer = const node_type*;
        using const_leaf_pointer = const leaf_node*;
        using const_inner_pointer = const inner_node*;
        using box_type = typename node_type::box_type;
        using point_type = typename node_type::point_type;
        using record_type = Record_;

        constexpr static auto capacity = M_;

    private:
        constexpr static auto max_left_rebuild_size = m_;
        constexpr static auto max_right_rebuild_size = M_ + 1 - m_;
        static_assert((M_ + 1) == max_left_rebuild_size + max_right_rebuild_size);

        struct Split {
            struct Item {
                box_type mbr;
                node_pointer node;
            };

            Item left;
            Item right;
        };

    public:
        node_pointer root = nullptr;

        ~RTree () { 
            if (root->is_leaf){
                delete root;
                return;
            }
            unsigned int n = std::thread::hardware_concurrency();
            size_t t = M_;
            size_t cnt = 0;
            while ( t > 0 ){                
                size_t s = n;
                if (t < n)
                    s = t;

                std::vector<std::thread> vecOfThreads;
                
                for (size_t i = cnt; i < (cnt + s); ++i){
                    node_pointer x = ((inner_node*)(root))->_records[i];
                    vecOfThreads.push_back(std::thread([x](){
                        delete x;
                    }));
                }

                for (std::thread & th : vecOfThreads){
                    if (th.joinable())
                        th.join();
                }
                cnt += s;
                t -= s;
            }

            for (size_t i = 0; i < M_; ++i) {
                auto* cur_box = reinterpret_cast<box_type*>((&(root->_boxes)[i]));
                cur_box->~box_type();
            }

            for ( auto a : (((inner_node*)(root))->_records) ){
                delete a;
            }
            
            root = nullptr;            
        }

        /* Inserts a box-record pair in the tree
         * Returns true if successful (it should always be successful tho...)
         * Usage:
         * >> Box
         * >> insert(*/
        bool insert (const box_type& box, const record_type& record) {
            if (!root) { // create a root node as a leaf with a single entry
                auto node = new leaf_node;
                root = static_cast<node_pointer>(node);

                // this should... always work. If it doesn't I... just don't know what you have done
                bool success = detail::_try_construct_at(node, box, record);
                assert(success);
                return true;
            }
            // call recursive function. This may propagate a split
            auto prop = _insert_helper(root, box, record);
            if (!prop.has_value()) {
                return true;
            }
            // split the root
            auto new_root = new inner_node;
            // these two should never fail. Otw something is wrong
            detail::_try_construct_at(new_root, prop->right.mbr, prop->right.node);
            detail::_try_construct_at(new_root, prop->left.mbr, prop->left.node);

            // replace the root node
            root = new_root;
            return true;
        }

        /* Gets all the records that intersect a query box in the tree.
         * Stores the result into the range starting at out.
         * Returns the end of the new range.
         * Usage:
         * >> std::vector<int> results;
         * >> tree.query({{1, 2, 3}, {4, 5, 6}}, std::back_inserter(results));
         * >> // Magically, results now contains all the _records
         */
        template<class OutputIter>
        OutputIter
        query (const box_type& box, OutputIter out) const {
            return _query_helper(root, box, out);
        }

    private:
        template<class OutputIter>
        OutputIter
        _query_helper (const_node_pointer base, const box_type& box, OutputIter out) const {
            if (base == nullptr) { // if nothing, welp... do nothing
                return out;
            }

            if (base->is_leaf) { // if leaf, just search on the local range for intersections
                auto node = static_cast<const_leaf_pointer>(base);
                for (size_t i = 0; i < node->size; ++i) {
                    if (geom::intersects(box, node->_boxes[i])) {
                        *out++ = node->_records[i];
                    }
                }
                return out;
            }
            // else, search on the subtrees whose mbr intersects the query box
            auto node = static_cast<const_inner_pointer>(base);
            for (size_t i = 0; i < node->size; ++i) {
                if (geom::intersects(box, node->_boxes[i])) {
                    out = _query_helper(node->_records[i], box, out);
                }
            }
            return out;
        }

        // Insert a mbr-record pair into the subtree rooted at base.
        // Returns `std::nullopt` (nothing) or a propagated split.
        std::optional<Split>
        _insert_helper (node_pointer base, const box_type& box, const record_type& record) {
            if (base->is_leaf) { // is a leaf
                auto node = static_cast<leaf_pointer>(base);
                auto inserted = detail::_try_construct_at(node, box, record);
                if (inserted) { // no split required
                    return std::nullopt;
                }
                // create split and propagate to caller
                return _handle_split_insert<leaf_node>(node, box, record);
            }
            // is an inner node
            auto node = static_cast<inner_pointer>(base);

            // pick the entry which requires minimal enlargement.
            size_t idx = detail::_min_enlargement_index(node->_boxes, node->size, box);

            // insert in that entry. This may propagate a split.
            auto prop = _insert_helper(node->_records[idx], box, record);

            if (!prop.has_value()) { // no split to propagate.
                // adjust the mbr
                node->_boxes[idx] = geom::join(node->_boxes[idx], box);
                return std::nullopt;
            }
            // split
            auto split = *prop;
            node->_boxes[idx] = split.left.mbr;
            node->_records[idx] = split.left.node;

            auto could_insert = detail::_try_construct_at(node, split.right.mbr, split.right.node);
            if (could_insert) { // no split required
                return std::nullopt;
            }
            // create split and propagate to caller
            return _handle_split_insert<inner_node>(
                node, split.right.mbr, split.right.node);
        }

        // insert an overflowing record into node.
        // This WILL (and MUST) cause a split.
        template<class Node>
        Split
        _handle_split_insert (
            Node* node,
            const box_type& box,
            const typename Node::record_type& record) {
            using std::begin, std::end;

            // move everything temporarily into this array, so that it's matched
            std::pair<box_type, typename Node::record_type> records[M_ + 1];
            for (size_t i = 0; i < M_; ++i) {
                records[i].first = std::move(node->_boxes[i]);
                records[i].second = std::move(node->_records[i]);
            }
            // also add the thing we want to insert there
            records[M_] = { box, record };

            // Get a set of nodes with maximal self-distance.
            // If you are wondering if this actually returns the most distant pair in the entire set... No it doesn't.
            // It returns a very close approximate tho.
            // And this function is already just a heuristic, so no need to do fancy stuff
            auto E1_idx = detail::_farthest_from_source<M_ + 1>(0, records);
            auto E2_idx = detail::_farthest_from_source<M_ + 1>(E1_idx, records);

            // Create a split with two groups. Reusing the node because... I can...
            node->size = 0;
            Split result{{ records[E1_idx].first, node },
                         { records[E2_idx].first, new Node }};

            assert(result.left.node->size == 0 && result.right.node->size == 0);

            // Distribute records into result.
            _distribute_groups<Node>(records, result);
            return result;
        }

        // Distribute the box-pair recs in `recs` into the split `out_groups`
        // WARNING: This alters the structure of out_groups. Beware of using it, outside regular splitting.
        // WARNING: This potentially alters memory directly. Any glitch/bug/segfault probably is happening here
        template<class Node>
        static void
        _distribute_groups (
            std::pair<box_type, typename Node::record_type> (& recs)[M_ + 1],
            Split& out_groups) {
            using geom::join;
            using geom::join_enlargement;
            using geom::area;

            const auto do_construct_on_choice = [] (auto& group, auto&& bx, auto&& rec) {
                auto group_node = static_cast<Node*>(group.node);
                group.mbr = join(group.mbr, bx);
                detail::_try_construct_at(
                    group_node,
                    std::forward<decltype(bx)>(bx),
                    std::forward<decltype(rec)>(rec));
            };

            for (auto&[bx, rec] : recs) {
                const bool left_full = out_groups.left.node->size == max_left_rebuild_size;
                const bool right_full = out_groups.right.node->size == max_right_rebuild_size;
                const auto left_enl = join_enlargement(out_groups.left.mbr, bx);
                const auto right_enl = join_enlargement(out_groups.right.mbr, bx);
                const auto left_area = area(out_groups.left.mbr);
                const auto right_area = area(out_groups.right.mbr);

                // Select the group that requires the least growth.
                // If a group is too full, it also skips it. Fuee!
                if (right_full || (!left_full && (left_enl < right_enl))) { // left group
                    do_construct_on_choice(out_groups.left, std::move(bx), std::move(rec));
                }
                else if (left_full || (!right_full && (right_enl < left_enl))) { // right group
                    do_construct_on_choice(out_groups.right, std::move(bx), std::move(rec));
                }
                else { // Tied, so choose the one with the smallest area
                    auto& min_group = right_area < left_area ? out_groups.right : out_groups.left;
                    do_construct_on_choice(min_group, std::move(bx), std::move(rec));
                }
            }
            assert(out_groups.left.node->size == max_left_rebuild_size);
            assert(out_groups.right.node->size == max_right_rebuild_size);
        }

    public:
        template<class OIter>
        OIter
        get_all (OIter out) const {
            return _get_all_helper(root, out);
        }

    private:
        template<class OIter>
        OIter
        _get_all_helper (const_node_pointer base, OIter out) const {
            if (base->is_leaf) { // is a leaf
                auto node = static_cast<const_leaf_pointer>(base);
                for (size_t i = 0; i < node->size; ++i) {
                    *out++ = node->_records[i];
                }
                return out;
            }
            auto node = static_cast<const_inner_pointer>(base);
            for (size_t i = 0; i < node->size; ++i) {
                out = _get_all_helper(node->_records[i], out);
            }
            return out;
        }
    };
}

#endif //EDA_RTREE_HPP
