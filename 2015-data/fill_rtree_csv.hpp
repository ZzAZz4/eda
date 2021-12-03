#include "../index/rtree.hpp"
#include "./csv-parser/csv.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace csv;

using point_type = geom::Point<double, 2>;
using box_type = geom::Box<point_type>;
using index_type = index::RTree<std::string, box_type, (8 << 10) / sizeof(box_type)>;

// (there are fancier ways to do this automatically with 
// preprocessor directives but for now this will do)

/* if __cpp_lib_experimental_filesystem */

// #include <experimental/filesystem>
// namespace fs = std::experimental::filesystem;

/* if __cpp_lib_filesystem */

#include <filesystem>

namespace fs = std::filesystem;

// Outputs a vector of file directories in a select path 
static std::vector<fs::path> files_in_folder (const fs::path& path) {
    std::vector<fs::path> fileNamePaths;
    if (!exists(path)) {
        return fileNamePaths;
    }

    for (const auto& entry : fs::directory_iterator(path)) {
        fileNamePaths.push_back(entry);
    }

    return fileNamePaths;
}

/* CSV STRUCTURE */

// fhv tripdata: IGNORE
// green tripdata:
// - node name: lpep_pickup_datetime
// - y: Pickup_longitude
// - x: Pickup_latitude
// yellow tripdata:
// - node name: tpep_pickup_datetime
// - y: pickup_longitude
// - x: pickup_latitude

// After insertion is implemented: Receives a 2d rtree storing std::string records,
// inserting the (lat, long) -> date&time data obtained from parsing the csv files.

bool rtree_filler (index_type& tree) {
    std::cout << "Current path is " << fs::current_path() << std::endl;
    std::vector<fs::path> vec = files_in_folder("../2015-data/data/");


    for (const fs::path& entry : vec) {
        CSVFormat format;
        format.header_row(0);
        format.delimiter(',');
        format.variable_columns(false);
        CSVReader reader(entry.u8string(), format);
        std::string file = fs::path(entry).filename().string();

        std::string pickup_label = file[0] == 'g' ? "lpep_pickup_datetime" : "tpep_pickup_datetime";
        std::string plon_label = file[0] == 'g' ? "Pickup_longitude" : "pickup_longitude";
        std::string plat_label = file[0] == 'g' ? "Pickup_latitude" : "pickup_latitude";

        std::cout << entry.string() << '\n';

        int i = 1;

        for (const auto& row : reader) {
            auto node_name = row[pickup_label].get<std::string>();
            auto y = row[plon_label].get<double>();
            auto x = row[plat_label].get<double>();

            point_type point{ y, x };
            box_type box{ point, point };
            tree.insert(box, node_name);
            if ((i - 1) % (256 * 256) == 0) {
                std::cout << i << std::endl;
            }
            i++;
        }

    }
    return true;
}
