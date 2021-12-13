
#ifndef EDA_SERIALIZER_HPP
#define EDA_SERIALIZER_HPP

#include <index/rtree_nodes.hpp>


namespace serial {
    template <class T>
    struct Serializer {
        using type = T;

        template<class Archive>
        static void serialize (const type& val, Archive& ar) {
            ar.write(reinterpret_cast<const char*>(&val), sizeof(type));
        }

        template<class Archive>
        static void recover (type& dump, Archive& ar) {
            ar.read(reinterpret_cast<char*>(&dump), sizeof(type));
        }
    };

    template<>
    struct Serializer<std::string> {
        using type = std::string;

        template<class Archive>
        static void serialize (const type& val, Archive& ar) {
            Serializer<typename type::size_type>::serialize(val.size(), ar);
            for (char i : val) {
                Serializer<char>::serialize(i, ar);
            }
        }

        template<class Archive>
        static void recover (type& dump, Archive& ar) {
            typename type::size_type size;
            Serializer<typename type::size_type>::recover(size, ar);
            dump.resize(size);
            for (char& i : dump) {
                Serializer<char>::recover(i, ar);
            }
        }
    };
}


#endif //EDA_SERIALIZER_HPP
