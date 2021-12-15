//#define GEOMETRY_COMPILE_TESTS
#include <fstream>
#include "index/rtree.hpp"
#include "index/serial/serializer.hpp"
#include "2015-data/fetch.hpp"
#include "index/serial/serial_query.hpp"

using point_type = geom::Point<float, 2>;
using box_type = geom::Box<point_type>;
using index_type = index_::RTree<std::string, box_type, 64>;

void
compile_to_index (const std::string& path, const std::string& ar_name, const std::string& dat_name) {
    std::cout << "Create\n";

    index_type index;
    fetch_locations<index_type>(path, index);

    std::cout << "Serialize\n";

    std::ofstream ar(ar_name, std::ios::binary);
    std::ofstream dat(dat_name, std::ios::binary);
    serial::Serializer<index_type>::serialize(index, ar, dat);

    std::cout << "Finished serializing!\n";
}

void read_index (const std::string& ar_name, const std::string& dat_name, const box_type& box) {
    std::cout << "Reload" << std::endl;

    index_type index;
    std::ifstream ar(ar_name, std::ios::binary);
    std::ifstream dat(dat_name, std::ios::binary);
    serial::Serializer<index_type>::recover(index, ar, dat);

    std::cout << "Reloaded" << std::endl;

    std::vector<std::string> res;
    index.query(box, std::back_inserter(res));
    for (const auto& i : res) {
        std::cout << i << '\n';
    }

    std::cout << "Finished read_index\n";
}

void just_query (const std::string& ar_name, const std::string& dat_name, const box_type& box) {
    std::ifstream ar(ar_name, std::ios::binary);
    std::ifstream dat(dat_name, std::ios::binary);

    std::vector<std::string> res;
    index_query<index_type>(ar, dat, box, std::back_inserter(res));

    for (const auto& i : res) {
        std::cout << i << '\n';
    }
}

int main () {
    std::string path = "../2015-data/data/";
    std::string archive_name = "../index_storage/tree.idx";
    std::string dat_name = "../index_storage/tree.dat";

    compile_to_index(path, archive_name, dat_name);

    box_type testbox({ -73.9589, 40.7168 }, { -73.9588, 40.7169 });

//    std::cout << "Test deserialize" << std::endl;
//    read_index(archive_name, dat_name, testbox);

    std::cout << "Just query" << std::endl;
    just_query(archive_name, dat_name, testbox);
}