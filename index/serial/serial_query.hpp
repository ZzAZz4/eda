#ifndef EDA_SERIAL_QUERY_HPP
#define EDA_SERIAL_QUERY_HPP

#include <index/rtree_nodes.hpp>
#include <index/serial/serializer.hpp>

template<class IndexType>
struct TemporaryNode {
    using box_type = typename IndexType::box_type;
    using node_type = typename IndexType::base_node;
    using record_type = typename IndexType::record_type;
    constexpr static auto capacity = IndexType::capacity;

    index_::detail::RTreeTag type;
    std::size_t size;
    box_type boxes[node_type::capacity];
    std::size_t child_positions[node_type::capacity];
};

template<class IndexType>
struct serial::Serializer<TemporaryNode<IndexType>> {
    using type = TemporaryNode<IndexType>;

    template<class Archive>
    static void recover (type& dump, Archive& ar) {
        Serializer<index_::detail::RTreeTag>::recover(dump.type, ar);
        Serializer<std::size_t>::recover(dump.size, ar);
        Serializer<typename type::box_type[type::capacity]>::recover(dump.boxes, ar);
        Serializer<std::size_t[type::capacity]>::recover(dump.child_positions, ar);
    }
};

template<class IndexType, class Archive, class DatFile, class OutIt>
OutIt index_query (Archive& ar, DatFile& dat, const typename IndexType::box_type& box, OutIt out);

template<class IndexType, class Archive, class DatFile, class OutIt>
OutIt
index_query_rec (
    const TemporaryNode<IndexType>& node,
    Archive& ar, DatFile& dat, const typename IndexType::box_type& box, OutIt out) {
    if (node.type == index_::detail::RTreeTag::LEAF) {
        typename IndexType::record_type record;

        for (size_t i = 0; i < node.size; ++i) {
            if (geom::intersects(box, node.boxes[i])) {
                dat.seekg(node.child_positions[i]);
                serial::Serializer<typename IndexType::record_type>::recover(record, dat);
                *out++ = record;
            }
        }
        return out;
    }
    TemporaryNode<IndexType> child;
    for (size_t i = 0; i < node.size; ++i) {
        if (geom::intersects(box, node.boxes[i])) {
            ar.seekg(node.child_positions[i]);
            serial::Serializer<TemporaryNode<IndexType>>::recover(child, ar);
            out = index_query_rec(child, ar, dat, box, out);
        }
    }
    return out;
}

template<class IndexType, class Archive, class DatFile, class OutIt>
OutIt
index_query (Archive& ar, DatFile& dat, const typename IndexType::box_type& box, OutIt out) {
    char not_empty;
    serial::Serializer<char>::recover(not_empty, ar);
    if (!not_empty) {
        return out;
    }
    using node_type = TemporaryNode<IndexType>;
    std::size_t position;
    node_type node;

    serial::Serializer<std::size_t>::recover(position, ar);
    ar.seekg(position);

    serial::Serializer<node_type>::recover(node, ar);
    return index_query_rec(node, ar, dat, box, out);
}

#endif //EDA_SERIAL_QUERY_HPP
