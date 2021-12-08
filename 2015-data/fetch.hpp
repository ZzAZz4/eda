#include "../index/rtree.hpp"
#include "./csv-parser/csv.hpp"
#include "./csv-parser/rapidcsv.h"
#include <iostream>
#include <string>
#include <vector>

using namespace csv;

// (there are fancier ways to do this automatically with 
// preprocessor directives but for now this will do)

/* if __cpp_lib_experimental_filesystem */

// #include <experimental/filesystem>
// namespace fs = std::experimental::filesystem;

/* if __cpp_lib_filesystem */

#include <filesystem>

namespace fs = std::filesystem;

// Outputs a vector of file directories in a select path 
static std::vector<fs::path>
files_in_folder (const fs::path& path) {
    std::vector<fs::path> fileNamePaths;
    if (!exists(path)) {
        return fileNamePaths;
    }

    for (const auto& entry : fs::directory_iterator(path)) {
        fileNamePaths.push_back(entry);
    }

    return fileNamePaths;
}

template<class Vec>
Vec try_fetch_locations (const fs::path& path);

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

template<class Vec>
Vec
fetch_locations (const fs::path& folder) {
    std::vector<fs::path> vec = files_in_folder(folder);
    Vec res;

    for (const fs::path& entry : vec) {
        Vec temp;
        // try {
        temp = try_fetch_locations<Vec>(entry);
   
        // } catch (std::runtime_error&) {
            // temp = try_fetch_locations<Vec>(entry, false);
        // }
        std::copy(temp.begin(), temp.end(), std::back_inserter(res));
    }
    return res;
}

template<class Vec>
Vec try_fetch_locations (const fs::path& path) {
    Vec res;
    auto out = std::back_inserter(res);

    // CSVFormat format;
    // format.header_row(0);
    // format.delimiter(',');
    // format.variable_columns(variable_cols);

    // CSVReader reader(path.u8string(), format);

    rapidcsv::Document doc(path.u8string());
    std::cout << "Read " << doc.GetRowCount() << " values." << std::endl;

    std::string file = path.filename().string();

    std::string pickup_label = file[0] == 'g' ? "lpep_pickup_datetime" : "tpep_pickup_datetime";
    std::string plon_label = file[0] == 'g' ? "Pickup_longitude" : "pickup_longitude";
    std::string plat_label = file[0] == 'g' ? "Pickup_latitude" : "pickup_latitude";

    std::cout << "File: " << file << std::endl;

    // for (const auto& row : reader) {
        // auto node_name = row[pickup_label].get<std::string>(); doc.GetCell<std::string>(pickup_label,i)
        // auto lat = row[plat_label].get<double>(); doc.GetCell<double>(plat_label,i)
        // auto lon = row[plon_label].get<double>(); doc.GetCell<double>(plon_label,i)

    for (long unsigned int i = 0; i < doc.GetRowCount(); ++i){
        auto node_name = doc.GetCell<std::string>(pickup_label,i);
        auto lat = doc.GetCell<double>(plat_label,i);
        auto lon = doc.GetCell<double>(plon_label,i);
        std::pair<point_type, std::string> record{{ lat, lon }, node_name };
        *out++ = record;
        // std::cout << doc.GetCell<std::string>(pickup_label,i) << ", " << doc.GetCell<std::string>(plat_label,i) << ", " << doc.GetCell<std::string>(plon_label,i) << "\n";
    }
    // doc.Clear();
    return res;
}
