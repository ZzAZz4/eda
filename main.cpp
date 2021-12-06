//#define GEOMETRY_COMPILE_TESTS
#include "index/rtree.hpp"
#include <iostream>
#include <vector>

using point_type = geom::Point<double, 2>;
using box_type = geom::Box<point_type>;
using index_type = index::RTree<std::string, box_type, 4>;

#include "2015-data/fetch.hpp"

index_type
create_index (const std::string& path) {
    using Vec = std::vector<std::pair<point_type, std::string>>;
    Vec records = fetch_locations<Vec>(path);
    std::multiset<std::string> mock;


    index_type tree;
    int i = 0;
    for (const auto&[point, name] : records) {
        auto box = box_type({ point[1], point[0] }, { point[1], point[0] });
        tree.insert(box, name);
        mock.insert(name);
        i++;
        if (i % 65536 == 0) {
            std::cout << i << std::endl;
        }
    }

    std::multiset<std::string> should_be;
    tree.get_all(std::inserter(should_be, should_be.end()));

    std::cout << should_be.size() << ' ' << records.size() << '\n';
    assert(should_be.size() == records.size());
    assert(mock == should_be);
    return tree;
}

int main () {
    index_type tree = create_index("../2015-data/j8/");
    std::vector<std::string> result;
    box_type box{{-73.937530517578125,40.804500579833984}, {-73.937530517578125,40.804500579833984}};
    tree.query(box, std::back_inserter(result));
    for (const auto& i : result) {
        std::cout << i << '\n';
    }
    return 0;
}