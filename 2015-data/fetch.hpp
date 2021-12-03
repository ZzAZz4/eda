#include "../index/rtree.hpp"
#include "./csv-parser/csv.hpp"
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

template<class OutIt>
OutIt try_fetch_locations (const fs::path& path, bool variable_cols, OutIt out);

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

template<class OutIt_>
OutIt_
fetch_locations (const fs::path& folder, OutIt_ out) {
    std::vector<fs::path> vec = files_in_folder(folder);

    for (const fs::path& entry : vec) {
        try {
            out = try_fetch_locations(entry, true, out);
        } catch (std::runtime_error&) {
            out = try_fetch_locations(entry, false, out);
        }
    }
    return out;
}

template<class OutIt>
OutIt try_fetch_locations (const fs::path& path, bool variable_cols, OutIt out) {
    CSVFormat format;
    format.header_row(0);
    format.delimiter(',');
    format.variable_columns(variable_cols);

    CSVReader reader(path.u8string(), format);
    std::string file = path.filename().string();

    std::string pickup_label = file[0] == 'g' ? "lpep_pickup_datetime" : "tpep_pickup_datetime";
    std::string plon_label = file[0] == 'g' ? "Pickup_longitude" : "pickup_longitude";
    std::string plat_label = file[0] == 'g' ? "Pickup_latitude" : "pickup_latitude";

    std::cout << "File: " << file << std::endl;

    for (const auto& row : reader) {
        auto node_name = row[pickup_label].get<std::string>();
        auto lat = row[plat_label].get<double>();
        auto lon = row[plon_label].get<double>();

        std::pair<point_type, std::string> record{{ lat, lon }, node_name };
        *out++ = record;
    }
    return out;
}
