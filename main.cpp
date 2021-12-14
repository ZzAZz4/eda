//#define GEOMETRY_COMPILE_TESTS
#include "index/rtree.hpp"
#include <iostream>
#include <vector>
#include <set>
#include <string>

using point_type = geom::Point<float, 2>;
using box_type = geom::Box<point_type>;
using index_type = index_::RTree<std::string, box_type, 9>;

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

t_type
create_index_ (const std::string& path) {
    t_type tree;
    fetch_locations_(path,tree);
    return tree;
}

int main () {
    float min_lon,min_lat,max_lon,max_lat;
    min_lon = -73.922592163085938;
    min_lat = 40.754528045654297; 
    max_lon = -73.922592163085938;
    max_lat = 40.754528045654297; 

    std::string sel;
    std::cout << "[(B)OOST, (I)MPLEMENTATION]: ";
    std::cin >> sel;
    
    if (sel[0] == 'B' || sel[0] == 'b')
    {

        std::cout << "BOOST RTREE:\n";

        auto t = create_index_("../2015-data/data/");
        
        // benchmarking
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        
        box query_box(point(min_lon, min_lat), point(max_lon, max_lat));
        std::vector<value> result_s;
        t.query(bgi::intersects(query_box), std::back_inserter(result_s));
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        std::cout << "Queries obtained in " << time_span.count() << " seconds." << std::endl;
        std::cout << "spatial query result:" << std::endl;

        BOOST_FOREACH(value const& v, result_s)
            std::cout << v.second << std::endl;
        std::cout << "\nexcecution finished" << '\n';
        
    }

    if (sel[0] == 'I' || sel[0] == 'i')
    {
        std::cout << "IMPLEMENTED RTREE:\n";

        index_type tree = create_index("../2015-data/data/");

        // benchmarking
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        std::vector<std::string> result;
        box_type box{{min_lon, min_lat}, {max_lon, max_lat}};
        tree.query(box, std::back_inserter(result));

        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        std::cout << "Queries obtained in " << time_span.count() << " seconds." << std::endl;
        

        for (const auto& i : result) {
            std::cout << i << '\n';
        }                                                               
        std::cout << "\nexcecution finished" << '\n';
    }


    return 0;
}