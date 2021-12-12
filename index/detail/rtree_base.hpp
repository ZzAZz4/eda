#ifndef EDA_RTREE_BASE_HPP
#define EDA_RTREE_BASE_HPP

#include <cstddef>
#include <optional>

namespace index_::detail {
    template<class Record_, class Box_, std::size_t M_, std::size_t m_, template<class> class>
    struct RTreeBase;

    template<class Record_, class Box_, std::size_t M_, std::size_t m_, template<class> class>
    struct RTreeLeaf;

    template<class Record_, class Box_, std::size_t M_, std::size_t m_, template<class> class>
    struct RTreeInner;

    template<class Record_, class Box_, std::size_t M_, std::size_t m_, template<class> class Ptr>
    struct RTreeBase {
    public:
        using base_type = RTreeBase;
        using leaf_type = RTreeLeaf<Record_, Box_, M_, m_, Ptr>;
        using inner_type = RTreeInner<Record_, Box_, M_, m_, Ptr>;

        using box_type = Box_;
        using point_type = typename box_type::point_type;
        using size_type = typename box_type::size_type;
        constexpr static std::size_t capacity = M_;
        constexpr static std::size_t lower_cnt = m_;

        using box_storage = box_type;
        using split_type = struct {
            struct Item_ { box_type mbr;base_type* node; };
            Item_ left, right;
        };

    public:
        explicit RTreeBase (RTreeTag type_) : type(type_) {}

        virtual ~RTreeBase () = default;

        std::optional<split_type>
        _insert_helper_dispatch (const box_type& box, const Record_& record);

        template<class OutputIter>
        OutputIter
        _query_helper_dispatch (const box_type& box, OutputIter out) const;

    public:
        const detail::RTreeTag type;
        size_type size = 0;
        box_storage boxes[capacity];
    };

}

#include "rtree_base_impl.hpp"

#endif //EDA_RTREE_BASE_HPP
