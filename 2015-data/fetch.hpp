#include "../index/rtree.hpp"
// #include "./csv-parser/csv.hpp"
// #include "./csv-parser/rapidcsv.h"
#include "./csv-parser/fast.h"
// #include <cstring>
#include <iostream>
#include <string>
#include <vector>

// using namespace csv;

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

template<class Vec, typename index_type>
Vec try_fetch_locations (const fs::path& path, index_type& tree);

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

template<class Vec, typename index_type>
Vec
fetch_locations (const fs::path& folder, index_type& tree) {
    std::vector<fs::path> vec = files_in_folder(folder);
    Vec res;

    for (const fs::path& entry : vec) {
        Vec temp;
        // try {
        temp = try_fetch_locations<Vec,index_type>(entry,tree);
   
        // } catch (std::runtime_error&) {
            // temp = try_fetch_locations<Vec>(entry, false);
        // }
        std::copy(temp.begin(), temp.end(), std::back_inserter(res));
    }
    return res;
}

template<class Vec, typename index_type>
Vec try_fetch_locations (const fs::path& path, index_type& tree) {
    Vec res;
    // auto out = std::back_inserter(res);
    // CSVFormat format;
    // format.header_row(0);
    // format.delimiter(',');
    // format.variable_columns(variable_cols);
    // CSVReader reader(path.u8string(), format);

    io::CSVReader<3,io::trim_chars<>, io::no_quote_escape<','>> in(path.u8string());
    std::string file = path.filename().string();

    std::string pickup_label = file[0] == 'g' ? "lpep_pickup_datetime" : "tpep_pickup_datetime";
    std::string plon_label = file[0] == 'g' ? "Pickup_longitude" : "pickup_longitude";
    std::string plat_label = file[0] == 'g' ? "Pickup_latitude" : "pickup_latitude";

    in.read_header(io::ignore_extra_column, pickup_label, plon_label, plat_label);
    if (file[0] == 'g'){
        in.col_order.push_back(-1);
        in.col_order.push_back(-1);
    }
    std::cout << "File: " << file << std::endl;

    // for (const auto& row : reader) {
        // auto node_name = row[pickup_label].get<std::string>(); doc.GetCell<std::string>(pickup_label,i)
        // auto lat = row[plat_label].get<float>(); doc.GetCell<float>(plat_label,i)
        // auto lon = row[plon_label].get<float>(); doc.GetCell<float>(plon_label,i)
    
    std::string node_name; float lon; float lat;
    int bad_row = 0;

    while(in.read_row(node_name, lon, lat)){
        if ( lon == 0 || lat == 0)
            bad_row++;
        else{
            auto box = box_type({ lon, lat }, { lon, lat });
            tree.insert(box, node_name);
        }

        // std::pair<point_type, std::string> record{{ lat, lon }, node_name };
        // *out++ = record;
    }
    std::cout << bad_row << " incomplete rows" << std::endl;
    return res;
}
