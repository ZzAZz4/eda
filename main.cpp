//#define GEOMETRY_COMPILE_TESTS
#include "index/rtree.hpp"
#include <iostream>
#include <vector>

using point_type = geom::Point<double, 2>;
using box_type = geom::Box<point_type>;
using index_type = index::RTree<std::string, box_type, 14>;

#include "2015-data/fetch.hpp"

index_type
create_index (const std::string& path) {
    std::vector<std::pair<point_type, std::string>> records;
    std::multiset<std::string> mock;
    fetch_locations(path, std::back_inserter(records));

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

    return tree;
}

int main () {
    index_type tree = create_index("../2015-data/data/");
    box_type query_box({ 40.8260994, -73.86501312 }, { 40.8260994, -73.86501312 });

    std::vector<std::string> result;
    auto inserter = std::back_inserter(result);
    tree.query(query_box, inserter);

    for (const auto& i : result) {
        std::cout << i << ' ';
    }


    return 0;
}