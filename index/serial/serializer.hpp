#ifndef EDA_INDEX_SERIALIZER_HPP
#define EDA_INDEX_SERIALIZER_HPP

#include <serial/serializer.hpp>


template<class Record_, class Box_, std::size_t M_, std::size_t m_>
struct serial::Serializer<index_::detail::RTreeBase<Record_, Box_, M_, m_>*> {
    using type = index_::detail::RTreeBase<Record_, Box_, M_, m_>*;
    using base = index_::detail::RTreeBase<Record_, Box_, M_, m_>;
    using leaf = typename base::leaf_type;
    using inner = typename base::inner_type;

    inline static std::size_t written = 0;

    template<class Archive>
    static void serialize (const type& node, Archive& ar) {
        Serializer<index_::detail::RTreeTag>::serialize(node->type, ar);
        Serializer<typename base::size_type>::serialize(node->size, ar);
        Serializer<typename base::box_type[base::capacity]>::serialize(node->boxes, ar);

        written++;
        if (written % 100000 == 0) {
            std::cout << "serializing... Written: " << written << std::endl;
            ar.flush();
        }

        switch (node->type) {
        case index_::detail::RTreeTag::LEAF: {

            auto cast = static_cast<leaf*>(node);
            for (size_t i = 0; i < node->size; ++i) {
                Serializer<typename leaf::record_type>::serialize(cast->records[i], ar);
            }
            return;
        }
        case index_::detail::RTreeTag::INNER: {
            auto inner_node = static_cast<inner*>(node);
            for (size_t i = 0; i < node->size; ++i) {
                Serializer<type>::serialize(inner_node->records[i], ar);
            }
            return;
        }
        default: throw std::out_of_range("Invalid type (wth?)");
        }
    }

    template<class Archive>
    static void recover (type& dump, Archive& ar) {
        index_::detail::RTreeTag tag;
        Serializer<index_::detail::RTreeTag>::recover(tag, ar);

        dump = tag == index_::detail::RTreeTag::LEAF ? (type) new leaf : (type) new inner;
        Serializer<typename base::size_type>::recover(dump->size, ar);
        Serializer<typename base::box_type[base::capacity]>::recover(dump->boxes, ar);

        switch (dump->type) {
        case index_::detail::RTreeTag::LEAF: {
            auto cast = static_cast<leaf*>(dump);
            for (size_t i = 0; i < dump->size; ++i) {
                Serializer<typename leaf::record_type>::recover(cast->records[i], ar);
            }

            return;
        }
        case index_::detail::RTreeTag::INNER: {
            auto inner_node = static_cast<inner*>(dump);
            for (size_t i = 0; i < dump->size; ++i) {
                Serializer<type>::recover(inner_node->records[i], ar);
            }
            return;
        }
        default: throw std::out_of_range("Invalid type (wth?)");
        }
    }
};

template<class Record_, class Box_, size_t M_, size_t m_>
struct serial::Serializer<index_::RTree<Record_, Box_, M_, m_>> {
    using type = index_::RTree<Record_, Box_, M_, m_>;

    template<class Archive>
    static void serialize (const type& val, Archive& ar) {
        Serializer<char>::serialize(val.root == nullptr ? '\0' : '\1', ar);
        if (val.root) {
            Serializer<typename type::base_node*>::serialize(val.root, ar);
        }
    }

    template<class Archive>
    static void recover (type& dump, Archive& ar) {
        char any_node;
        Serializer<char>::recover(any_node, ar);
        if (any_node) {
            Serializer<typename type::base_node*>::recover(dump.root, ar);
        }
    }
};

namespace index_ {

}

#endif //EDA_SERIALIZER_HPP
