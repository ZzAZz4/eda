//#define GEOMETRY_COMPILE_TESTS
#include <fstream>
#include "index/rtree.hpp"
#include "index/serial/serializer.hpp"
#include "2015-data/fetch.hpp"

using point_type = geom::Point<float, 2>;
using box_type = geom::Box<point_type>;
using index_type = index_::RTree<std::string, box_type, 64>;

void write() {
    std::cout << "Create\n";
    index_type index;
    fetch_locations<index_type>("../2015-data/data/", index);
    std::cout << "Created: " << index_type::created << "entries" << std::endl;
    std::cout << "Inserted: " << index_type::base_node::allocated << "entries" << std::endl;

    std::vector<std::string> res;
    index.query(box_type({-73.958816528320312,40.716823577880859}, {-73.958816528320312,40.716823577880859}), std::back_inserter(res));
    for (const auto &i : res) std::cout << i << '\n';

    std::cout << "Serialize\n";
    std::ofstream os("../tree.bin", std::ios::binary);
    serial::Serializer<index_type>::serialize(index, os);
    std::cout << "Destroy!\n";
}

void read() {
    std::cout << "Reload" << std::endl;
    index_type index;
    std::ifstream is("../tree.bin", std::ios::binary);
    serial::Serializer<index_type>::recover(index, is);
    std::cout << "Reloaded" << std::endl;

    std::vector<std::string> res;
    index.query(box_type({-73.958816528320312,40.716823577880859}, {-73.958816528320312,40.716823577880859}), std::back_inserter(res));
    for (const auto &i : res) std::cout << i << '\n';
}

int main () {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    write();
    std::cout << "Allocation check: " << index_type::base_node::allocated << std::endl;
    read();
}