//#define GEOMETRY_COMPILE_TESTS
#include "index/rtree.hpp"
#include "2015-data/fetch.hpp"
#include <iostream>
#include <vector>
#include <string>

using point_type = geom::Point<float, 2>;
using box_type = geom::Box<point_type>;
using index_type = index_::RTree<std::string, box_type, 64>;

index_type
create_index (const std::string& path) {
    index_type tree;
    fetch_locations<index_type>(path,tree);
    return tree;
}

int main () {
    index_type tree = create_index("../2015-data/j8/");
    std::vector<std::string> result;
    box_type box{{-73.958816528320312,40.716823577880859}, {-73.958816528320312,40.716823577880859}};
    tree.query(box, std::back_inserter(result));
    for (const auto& i : result) {
        std::cout << i << '\n';
    }
    std::cout << "excecution finished" << '\n';
    return 0;
}