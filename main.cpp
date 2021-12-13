//#define GEOMETRY_COMPILE_TESTS
#include <fstream>
#include "index/rtree.hpp"
#include "index/serial/serializer.hpp"
#include "2015-data/fetch.hpp"

using point_type = geom::Point<float, 2>;
using box_type = geom::Box<point_type>;
using index_type = index_::RTree<std::string, box_type, 3>;

void write() {
    std::cout << "Create\n";
    index_type index;
    fetch_locations<index_type>("../2015-data/j8/", index);

    std::vector<std::string> res;
    index.query(box_type({-73.958816528320312,40.716823577880859}, {-73.958816528320312,40.716823577880859}), std::back_inserter(res));
    for (const auto &i : res) std::cout << i << '\n';

    std::ofstream os("../tree.bin", std::ios::binary);
    serial::Serializer<index_type>::serialize(index, os);
}

void read() {
    std::cout << "Reload\n";
    index_type index;
    std::ifstream is("../tree.bin", std::ios::binary);
    serial::Serializer<index_type>::recover(index, is);

    std::vector<std::string> res;
    index.query(box_type({-73.958816528320312,40.716823577880859}, {-73.958816528320312,40.716823577880859}), std::back_inserter(res));
    for (const auto &i : res) std::cout << i << '\n';
}

int main () {
    write();
    read();
}