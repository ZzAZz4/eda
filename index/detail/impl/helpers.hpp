//
// Created by Esteban on 12/11/2021.
//

#ifndef EDA_HELPERS_HPP
#define EDA_HELPERS_HPP

#include <geometry/ops.hpp>

namespace index_::detail {
    template<size_t M_, class BoxPair>
    static size_t
    _farthest_from_source (size_t src, BoxPair (& recs)[M_]) {
        const auto& src_box = recs[src].first;
        std::size_t res_idx = 0;
        double res_distance = geom::min_sq_distance(src_box, recs[0].first);

        for (std::size_t i = 1; i < M_; ++i) {
            auto cur_distance = geom::min_sq_distance(src_box, recs[i].first);
            if (cur_distance > res_distance) {
                res_distance = cur_distance;
                res_idx = i;
            }
        }
        return res_idx;
    }

    template<class Node, class Base, class Box = typename Node::box_type, class Record = typename Node::record_type>
    static bool
    _try_add (Base* base, const Box& box, const Record& record) {
        Node* node = static_cast<Node*> (base);
        if (node->size == node->capacity) {
            return false;
        }
        node->boxes[node->size] = box;
        node->records[node->size] = record;
        node->size++;
        return true;
    }

    template<class It, class Box>
    static size_t
    _min_enlargement_index (It first, size_t size, const Box& box) {
        using geom::join_enlargement;

        It last = first + size;
        It best = first;
        auto best_enl = join_enlargement(*first, box);

        for (auto it = std::next(first); it != last; ++it) {
            auto cur_enl = join_enlargement(*it, box);
            if (cur_enl < best_enl) {
                best = it;
                best_enl = cur_enl;
            }
        }
        return std::distance(first, best);
    }


    template<class Node, class Box = typename Node::box_type, class Record = typename Node::record_type, std::size_t M_ = Node::capacity>
    void
    _distribute_groups (
        std::pair<Box, Record> (& entries)[M_ + 1], typename Node::split_type& out) {
        using geom::join;
        using geom::join_enlargement;
        using geom::area;

        constexpr auto LEFT_TARGET_SIZE = Node::lower_cnt;
        constexpr auto RIGHT_TARGET_SIZE = Node::capacity + 1 - Node::lower_cnt;

        auto move_to_group = [] (auto& group, auto&& bx, auto&& rec) {
            group.mbr = join(group.mbr, bx);
            detail::_try_add<Node>(group.node, bx, rec);
        };

        for (auto&[box, record] : entries) {
            bool left_full = out.left.node->size == LEFT_TARGET_SIZE;
            bool right_full = out.right.node->size == RIGHT_TARGET_SIZE;

            if (right_full) { // left group
                move_to_group(out.left, std::move(box), std::move(record));
                continue;
            }
            else if (left_full) {
                move_to_group(out.right, std::move(box), std::move(record));
                continue;
            }
            auto left_enl = join_enlargement(out.left.mbr, box);
            auto right_enl = join_enlargement(out.right.mbr, box);

            if (left_enl < right_enl) {
                move_to_group(out.left, std::move(box), std::move(record));
                continue;
            }
            else if (right_enl < left_enl) {
                move_to_group(out.right, std::move(box), std::move(record));
                continue;
            }
            auto left_area = area(out.left.mbr);
            auto right_area = area(out.right.mbr);
            auto& min_group = right_area < left_area ? out.right : out.left;
            move_to_group(min_group, std::move(box), std::move(record));
        }
    }

    template<class Node, class Box = typename Node::box_type, class Record = typename Node::record_type, std::size_t M_ = Node::capacity>
    typename Node::split_type
    _handle_split_insert (Node* node, const Box& box, const Record& record) {
        std::pair<typename Node::box_type, typename Node::record_type> records[M_ + 1];
        for (size_t i = 0; i < M_; ++i) {
            records[i].first = std::move(node->boxes[i]);
            records[i].second = std::move(node->records[i]);
        }
        records[M_] = { box, record };

        auto E1_idx = detail::_farthest_from_source<M_ + 1>(0, records);
        auto E2_idx = detail::_farthest_from_source<M_ + 1>(E1_idx, records);

        // Create a split with two groups. Reusing the node because... I can...
        node->size = 0;
        typename Node::split_type result{{ records[E1_idx].first, node },
                                         { records[E2_idx].first, new Node }};

        _distribute_groups<Node>(records, result);
        return result;
    }

}


#endif //EDA_HELPERS_HPP
