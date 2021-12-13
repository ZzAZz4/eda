// #include "./csv-parser/csv.hpp"
// #include "./csv-parser/rapidcsv.h"
#include "./csv-parser/fast.h"
#include <iostream>
#include <string>
#include <chrono>

#if __cpp_lib_experimental_filesystem
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
#endif


namespace fs = std::filesystem;

// Outputs a vector of file directories in a select path 
std::vector<fs::path>
files_in_folder (const fs::path& path) {
    std::vector<fs::path> fileNamePaths;
    if (!fs::exists(path)) {
        throw std::runtime_error("ERROR: location does not exist" + path.string());
    }

    std::copy(fs::directory_iterator(path), fs::directory_iterator(),
              std::back_inserter(fileNamePaths));

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


template<typename index_type>
void try_fetch_locations (const fs::path& path, index_type& tree) {
    io::CSVReader<3, io::trim_chars<>, io::no_quote_escape<','>> in(path.u8string());
    std::string file = path.filename().string();

    std::string pickup_label = file[0] == 'g' ? "lpep_pickup_datetime" : "tpep_pickup_datetime";
    std::string plon_label = file[0] == 'g' ? "Pickup_longitude" : "pickup_longitude";
    std::string plat_label = file[0] == 'g' ? "Pickup_latitude" : "pickup_latitude";

    in.read_header(io::ignore_extra_column, pickup_label, plon_label, plat_label);
    if (file[0] == 'g') {
        std::string a = std::string(1, file[20]);
        std::string b = std::string(1, file[21]);
        if (std::stoi(a + b) < 7) {
            in.col_order.push_back(-1);
            in.col_order.push_back(-1);
        }
    }
    std::cout << "File: " << file << std::endl;

    std::string node_name;
    double lon;
    double lat;
    int bad_row = 0;

    while (in.read_row(node_name, lon, lat)) {
        if (lon == 0 || lat == 0) {
            bad_row++;
        }
        else {
            typename index_type::point_type point{ (float) lon, (float) lat };
            typename index_type::box_type box(point, point);
            tree.insert(box, node_name);
        }
    }
    std::cout << bad_row << " incomplete rows" << std::endl;
}


template<typename index_type>
void
fetch_locations (const fs::path& folder, index_type& tree) {
    std::vector<fs::path> vec = files_in_folder(folder);
    // benchmarking
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    for (const fs::path& entry : vec) {
        try_fetch_locations<index_type>(entry, tree);
    }
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(
        t2 - t1);

    std::cout << "Records inserted in " << time_span.count() << " seconds." << std::endl;
}