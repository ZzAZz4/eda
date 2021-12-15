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

    template<class Archive, class DatFile>
    static std::size_t serialize (const type& node, Archive& ar, DatFile& dat) {
        written++;
        if (written % 10000 == 0) {
            std::cout << "Serialized " << written << " nodes" << '\n';
        }

        switch (node->type) {
        case index_::detail::RTreeTag::LEAF: {
            auto position = ar.tellp();
            Serializer<index_::detail::RTreeTag>::serialize(node->type, ar);
            Serializer<typename base::size_type>::serialize(node->size, ar);
            Serializer<typename base::box_type[base::capacity]>::serialize(node->boxes, ar);

            auto cast = static_cast<leaf*>(node);
            for (std::size_t i = 0; i < node->size; ++i) {
                std::size_t data_pos = dat.tellp();
                Serializer<typename leaf::record_type>::serialize(cast->records[i], dat);
                Serializer<std::size_t>::serialize(data_pos, ar);
            }
            return position;
        }
        case index_::detail::RTreeTag::INNER: {
            auto inner_node = static_cast<inner*>(node);
            std::size_t child_positions[base::capacity];
            for (std::size_t i = 0; i < node->size; ++i) {
                auto child_position = Serializer<type>::serialize(inner_node->records[i], ar, dat);
                child_positions[i] = child_position;
            }
            auto position = ar.tellp();
            Serializer<index_::detail::RTreeTag>::serialize(node->type, ar);
            Serializer<typename base::size_type>::serialize(node->size, ar);
            Serializer<typename base::box_type[base::capacity]>::serialize(node->boxes, ar);
            Serializer<std::size_t[base::capacity]>::serialize(child_positions, ar);
            return position;
        }
        default: throw std::out_of_range("Invalid type (wth?)");
        }
    }

    template<class Archive, class DatFile>
    static void recover (type& dump, Archive& ar, DatFile& dat) {
        index_::detail::RTreeTag tag;
        Serializer<index_::detail::RTreeTag>::recover(tag, ar);

        std::size_t children_pos[base::capacity];
        dump = tag == index_::detail::RTreeTag::LEAF ? (type) new leaf : (type) new inner;
        Serializer<typename base::size_type>::recover(dump->size, ar);
        Serializer<typename base::box_type[base::capacity]>::recover(dump->boxes, ar);
        Serializer<std::size_t[base::capacity]>::recover(children_pos, ar);

        switch (dump->type) {
        case index_::detail::RTreeTag::LEAF: {
            auto cast = static_cast<leaf*>(dump);
            for (std::size_t i = 0; i < dump->size; ++i) {
                dat.seekg(children_pos[i]);
                Serializer<typename leaf::record_type>::recover(cast->records[i], dat);
            }
            return;
        }
        case index_::detail::RTreeTag::INNER: {
            auto cast = static_cast<inner*>(dump);
            for (std::size_t i = 0; i < dump->size; ++i) {
                ar.seekg(children_pos[i]);
                Serializer<typename inner::record_type>::recover(cast->records[i], ar, dat);
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

    template<class Archive, class DatFile>
    static void serialize (const type& val, Archive& ar, DatFile& dat) {
        Serializer<char>::serialize(val.root == nullptr ? '\0' : '\1', ar);
        if (val.root) {
            Serializer<std::size_t>::serialize(0, ar);
            std::size_t position = Serializer<typename type::base_node*>::serialize(val.root, ar, dat);
            ar.seekp(1);
            Serializer<std::size_t>::serialize(position, ar);
        }
    }

    template<class Archive, class DatFile>
    static void recover (type& dump, Archive& ar, DatFile& dat) {
        char any_node;
        std::size_t root_pos;
        Serializer<char>::recover(any_node, ar);
        if (any_node) {
            Serializer<std::size_t>::recover(root_pos, ar);
            ar.seekg(root_pos);
            Serializer<typename type::base_node*>::recover(dump.root, ar, dat);
        }
    }
};

namespace index_ {

}

#endif //EDA_SERIALIZER_HPP
