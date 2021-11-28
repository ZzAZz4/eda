#ifndef EDA_RTREE_HPP
#define EDA_RTREE_HPP

#include "rtree_node.hpp"
#include "../geometry/ops.hpp"


namespace index {
    struct not_implemented : std::exception {
        [[nodiscard]] const char*
        what () const noexcept override { return "Not implemented!\n"; }
    };

    template<class Record_,
        class Box_,
        std::size_t M_,
        std::size_t m_ = M_ / 2>
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

    private:
        struct Split {
            struct Item { box_type mbr; node_pointer node; };

            Item left;
            Item right;
        };

    public:
        node_pointer root = nullptr;

        ~RTree () { delete root; }

        /* Inserts a box-record pair in the tree
         * Returns true if successful (it should always be successful tho...)
         * Usage:
         * >> Box
         * >> insert(*/
        bool
        insert (const box_type& box, const record_type& record) {
            if (!root) { // create a root node as a leaf with a single entry
                auto node = new leaf_node;
                root = static_cast<node_pointer>(node);

                // this should... always work. If it doesn't I... just don't know what you have done
                bool success = _try_construct_at(node, box, record);
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
            _try_construct_at(new_root, prop->right.mbr, prop->right.node);
            _try_construct_at(new_root, prop->left.mbr, prop->left.node);

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
        _query_helper (
            const_node_pointer base,
            const box_type& box, OutputIter out) const {
                
            if (base == nullptr) return out;

            if (base->_c_is_leaf) {
                auto node = static_cast<const_leaf_pointer>(base);
                for (size_t i = 0; i < node->size; ++i) {
                    if (geom::intersects(box, node->_boxes[i])) {
                        *out++ = node->_records[i];
                    }
                }
                return out;
            }
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
        _insert_helper (
            node_pointer base,
            const box_type& box,
            const record_type& record
        ) {
            if (base->_c_is_leaf) { // is a leaf
                auto node = static_cast<leaf_pointer>(base);
                auto inserted = _try_construct_at(node, box, record);
                if (inserted) { // no split required
                    return std::nullopt;
                }
                // create split and propagate to caller
                return _handle_split_insert<leaf_node>(node, box, record);
            }
            // is an inner node
            auto node = static_cast<inner_pointer>(base);

            // pick the entry which requires minimal enlargement.
            size_t idx = min_enlargement_index(node->_boxes, node->size, box);

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

            auto could_insert = _try_construct_at(
                node, split.right.mbr, split.right.node);
            if (could_insert) { // no split required
                return std::nullopt;
            }
            // create split and propagate to caller
            return _handle_split_insert<inner_node>(
                node, split.right.mbr, split.right.node);
        }

        template<class It>
        size_t
        min_enlargement_index (It begin, size_t size, const box_type& box) {
            using geom::join_enlargement;
            auto it = std::min_element(
                begin, begin + size, [&box] (const auto& a, const auto& b) {
                    return join_enlargement(a, box) < join_enlargement(b, box);
                });
            auto index = std::distance(begin, it);
            return index;
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
            auto E1_idx = _farthest_from_source(0, records);
            auto E2_idx = _farthest_from_source(E1_idx, records);

            // Create a split with two groups. Reusing the node because... I can...
            node->size = 0;
            Split result{{ records[E1_idx].first, node },
                         { records[E2_idx].first, new Node }};

            // Distribute records into result.
            _distribute_groups<Node>(records, result);
            return result;
        }

        // Distribute the box-pair recs in `recs` into the split `out_groups`
        // WARNING: This alters the structure of out_groups. Beware of using it, outside regular splitting.
        // WARNING: This potentially alters memory directly. Any glitch/bug/segfault probably is happening here
        template<class Node>
        void
        _distribute_groups (
            std::pair<box_type, typename Node::record_type> (& recs)[M_ + 1],
            Split& out_groups) {
            auto left = static_cast<Node*>(out_groups.left.node);
            auto right = static_cast<Node*>(out_groups.right.node);

            using geom::join;
            using geom::join_enlargement;
            using geom::area;

            for (auto&[bx, rec] : recs) {
                // calculate growth per group
                auto left_enl = join_enlargement(out_groups.left.mbr, bx);
                auto right_enl = join_enlargement(out_groups.right.mbr, bx);

                // Select the group that requires the least growth.
                // If a group is too full, it also skips it
                if (left_enl < right_enl || right->size >= m_) { // left group
                    out_groups.left.mbr = join(out_groups.left.mbr, bx);
                    _try_construct_at(left, std::move(bx), std::move(rec));
                    continue;
                }
                if (right_enl < left_enl || left->size >= m_) { // right group
                    out_groups.right.mbr = join(out_groups.right.mbr, bx);
                    _try_construct_at(right, std::move(bx), std::move(rec));
                    continue;
                }
                // Tied, so choose the one with the smallest area
                auto left_area = area(out_groups.left.mbr);
                auto right_area = area(out_groups.right.mbr);

                auto& min_group = right_area < left_area ? out_groups.right
                                                         : out_groups.left;

                min_group.mbr = join(min_group.mbr, bx);
                auto child_node = static_cast<Node*>(min_group.node);
                _try_construct_at(child_node, std::move(bx), std::move(rec));
            }
        }

        template<class BoxPair>
        size_t _farthest_from_source (size_t src, BoxPair (& recs)[M_ + 1]) {
            using geom::min_sq_distance;
            const auto& src_box = recs[src].first;

            auto it = std::max_element(
                recs, recs + M_ + 1, [&src_box] (const auto& a, const auto& b) {
                    return min_sq_distance(src_box, a.first) <
                           min_sq_distance(src_box, b.first);
                });

            auto res_idx = std::distance(recs, it);
            return res_idx;
        }

        template<class Node>
        bool
        _try_construct_at (
            Node* node,
            const box_type& box,
            const typename Node::record_type& record) {
            if (node->size == node->capacity) {
                return false;
            }
            node->_boxes[node->size] = box;
            node->_records[node->size] = record;
            node->size++;
            return true;
        }
    };
}

#endif //EDA_RTREE_HPP
