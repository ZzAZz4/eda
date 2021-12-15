//#define GEOMETRY_COMPILE_TESTS
#include <fstream>
#include "index/rtree.hpp"
#include "index/serial/serializer.hpp"
#include "2015-data/fetch.hpp"
#include "index/serial/serial_query.hpp"

using point_type = geom::Point<float, 2>;
using box_type = geom::Box<point_type>;
using index_type = index_::RTree<std::string, box_type, 64>;

void write () {
    std::cout << "Create\n";
    index_type index;
    fetch_locations<index_type>("../2015-data/j8/", index);
    std::cout << "Created: " << index_type::created << "entries" << std::endl;
    std::cout << "Inserted: " << index_type::base_node::allocated << "entries" << std::endl;

    std::vector<std::string> res;
    index.query(
        box_type(
            { -73.922592163085938, 40.754528045654297 },
            { -73.922592163085938, 40.754528045654297 }),
        std::back_inserter(res));
    for (const auto& i : res) {
        std::cout << i << '\n';
    }

    std::cout << "Serialize\n";
    std::ofstream ar("../tree.bin", std::ios::binary);
    std::ofstream dat("../tree.dat", std::ios::binary);
    serial::Serializer<index_type>::serialize(index, ar, dat);
    std::cout << "Destroy!\n";
}

void read () {
    std::cout << "Reload" << std::endl;
    index_type index;
    std::ifstream ar("../tree.bin", std::ios::binary);
    std::ifstream dat("../tree.dat", std::ios::binary);
    serial::Serializer<index_type>::recover(index, ar, dat);
    std::cout << "Reloaded" << std::endl;

    std::vector<std::string> res;
    index.query(
        box_type(
            { -73.922592163085938, 40.754528045654297 },
            { -73.922592163085938, 40.754528045654297 }),
        std::back_inserter(res));
    for (const auto& i : res) {
        std::cout << i << '\n';
    }
}

void just_query () {
    std::cout << "Just query" << std::endl;
    std::ifstream ar("../tree.bin", std::ios::binary);
    std::ifstream dat("../tree.dat", std::ios::binary);
    box_type box(
        { -73.958816528320312, 40.716823577880859 }, { -73.958816528320312, 40.716823577880859 });

    std::vector<std::string> res;
    index_query<index_type>(ar, dat, box, std::back_inserter(res));

    for (const auto& i : res) {
        std::cout << i << '\n';
    }
}

int main () {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // write();
    std::cout << "Allocation check: " << index_type::base_node::allocated << std::endl;
//    read();
    just_query();
}