#ifndef EDA_RTREE_NODE_HPP
#define EDA_RTREE_NODE_HPP

#include <cstddef>
#include <cassert>
#include "../geometry/box.hpp"


namespace index::detail {
    template<class Record_, class Box_, std::size_t M_, std::size_t m_>
    struct RTreeBase;

    template<class Record_, class Box_, std::size_t M_, std::size_t m_>
    struct RTreeLeaf;

    template<class Record_, class Box_, std::size_t M_, std::size_t m_>
    struct RTreeInner;

    template<class Record_, class Box_, std::size_t M_, std::size_t m_>
    struct RTreeBase {
        using base_type = RTreeBase;
        using leaf_type = RTreeLeaf<Record_, Box_, M_, m_>;
        using inner_type = RTreeInner<Record_, Box_, M_, m_>;

        using box_type = Box_;
        using point_type = typename box_type::point_type;
        using size_type = typename box_type::size_type;
        constexpr static std::size_t capacity = M_;
        constexpr static std::size_t lower_cnt = m_;

        using box_storage = box_type;

        const bool _c_is_leaf;
        size_type size = 0;
        box_storage _boxes[capacity];

        explicit RTreeBase (bool is_leaf) : _c_is_leaf(is_leaf) {}

        ~RTreeBase () {
            for (size_t i = 0; i < size; ++i) {
                auto* cur_box = reinterpret_cast<box_type*>((&_boxes[i]));
                cur_box->~box_type();
            }
            if (_c_is_leaf) ((leaf_type*)(this))->cleanup();
            else ((inner_type*)(this))->cleanup();
        }
    };

    template<class Record_, class Box_, std::size_t M_, std::size_t m_>
    struct RTreeLeaf : public RTreeBase<Record_, Box_, M_, m_> {
        using base_type = RTreeBase<Record_, Box_, M_, m_>;
        using box_type = typename base_type::box_type;
        using point_type = typename base_type::point_type;
        using size_type = typename base_type::size_type;
        using box_storage = typename base_type::box_storage;
        constexpr static std::size_t capacity = base_type::capacity;
        constexpr static std::size_t lower_cnt = base_type::lower_cnt;

        using record_type = Record_;
        using record_storage = record_type;

        record_storage _records[capacity];

        RTreeLeaf () : base_type(true) {}

        void cleanup() {
            for (size_t i = 0; i < this->size; ++i) {
                ((record_type *)(&this->_records[i]))->~record_type();
                std::cout << "Delete record\n";
            }
            std::cout << "Delete leaf\n";
        }
    };

    template<class Record_, class Box_, std::size_t M_, std::size_t m_>
    struct RTreeInner : public RTreeBase<Record_, Box_, M_, m_> {
        using base_type = RTreeBase<Record_, Box_, M_, m_>;
        using box_type = typename base_type::box_type;
        using point_type = typename base_type::point_type;
        using size_type = typename base_type::size_type;

        constexpr static std::size_t capacity = base_type::capacity;
        constexpr static std::size_t lower_cnt = base_type::lower_cnt;
        using record_type = base_type*;

        using typename base_type::box_storage;

        record_type _records[capacity];

        RTreeInner () : base_type(false) {}

        void cleanup() {
            for (size_t i = 0; i < this->size; ++i) {
                auto* cur_ch = reinterpret_cast<record_type>(_records[i]);
                delete cur_ch;
            }
            std::cout << "Delete inner\n";
        }
    };
}

#endif //EDA_RTREE_NODE_HPP
