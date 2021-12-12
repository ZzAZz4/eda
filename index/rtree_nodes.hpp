#ifndef EDA_RTREE_NODES_HPP
#define EDA_RTREE_NODES_HPP

namespace index_::detail {
    enum class RTreeTag : uint8_t {
        LEAF = 0,
        INNER = 1,
        UNLOADED = 2
    };
}

#include "detail/rtree_base.hpp"
#include "detail/rtree_leaf.hpp"
#include "detail/rtree_inner.hpp"

#endif //EDA_RTREE_NODES_HPP
