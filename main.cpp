//#define GEOMETRY_COMPILE_TESTS
#include "index/rtree.hpp"
#include <iostream>
#include <vector>
#include <set>
#include <string>

using point_type = geom::Point<float, 2>;
using box_type = geom::Box<point_type>;
using index_type = index_::RTree<std::string, box_type, 16384>;

#include "2015-data/fetch.hpp"
#include "benchmark/bench.hpp"

#include <boost/foreach.hpp>

index_type
create_index (const std::string& path) {
    using Vec = std::vector<std::pair<point_type, std::string>>;
    index_type tree;

    fetch_locations<Vec,index_type>(path,tree);
    return tree;
}


bgi::rtree< value, bgi::quadratic<16> > 
create_index_ (const std::string& path) {
    auto a = fetch_locations_(path);
    return a;
}

int main () {

    {
        std::cout << "BOOST RTREE:\n";

        auto t = create_index_("../2015-data/data/");
        
        // find values intersecting some area defined by a box
        box query_box(point(-73.922592163085938, 40.754528045654297), point(-73.922592163085938, 40.754528045654297));
        std::vector<value> result_s;
        t.query(bgi::intersects(query_box), std::back_inserter(result_s));

        std::cout << "spatial query result:" << std::endl;
        for (auto a : result_s){
            std::cout << 'x';
        }

        BOOST_FOREACH(value const& v, result_s)
            std::cout << v.second << std::endl;
    }

    std::cout << "_____________________________________________________" << std::endl;

    {
        std::cout << "IMPLEMENTED RTREE:\n";

        index_type tree = create_index("../2015-data/data/");

        std::vector<std::string> result;
        box_type box{{-73.922592163085938,40.754528045654297}, {-73.922592163085938,40.754528045654297}};
        tree.query(box, std::back_inserter(result));

        for (const auto& i : result) {
            std::cout << i << '\n';
        }
    
    }

    std::cout << "excecution finished" << '\n';

    return 0;
}