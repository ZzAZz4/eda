//#define GEOMETRY_COMPILE_TESTS
#include "index/rtree.hpp"
#include "2015-data/fetch.hpp"

using point_type = geom::Point<float, 2>;
using box_type = geom::Box<point_type>;
using index_type = index_::RTree<std::string, box_type, 3>;


int main () {
    index_type index;
    fetch_locations<index_type>("../2015-data/j8/", index);

    std::vector<std::string> res;
    index.query(box_type({-73.958816528320312,40.716823577880859}, {-73.958816528320312,40.716823577880859}), std::back_inserter(res));
    for (const auto &i : res) std::cout << i << '\n';
}