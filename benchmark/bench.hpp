#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

// to store queries results
#include <vector>
#include <string>

// just for output
#include <iostream>
#include "../2015-data/csv-parser/fast.h"

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<box, std::string> value;

#include <filesystem>
namespace fs = std::filesystem;

using t_type = bgi::rtree< value, bgi::quadratic<16> >;

// Outputs a vector of file directories in a select path 
static std::vector<fs::path>
files_in_folder_ (const fs::path& path) {
    std::vector<fs::path> fileNamePaths;
    if (!exists(path)) {
        return fileNamePaths;
    }

    for (const auto& entry : fs::directory_iterator(path)) {
        fileNamePaths.push_back(entry);
    }

    return fileNamePaths;
}


void try_fetch_locations_ (const fs::path& path, t_type& rtree);

void
fetch_locations_ (const fs::path& folder, t_type& rtree) {
    std::vector<fs::path> vec = files_in_folder_(folder);
    
    // benchmarking
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    for (const fs::path& entry : vec) {
        try_fetch_locations_(entry,rtree);
    }
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "Records inserted in " << time_span.count() << " seconds." << std::endl;
    return;
}

void try_fetch_locations_ (const fs::path& path, t_type& rtree) {

    io::CSVReader<3,io::trim_chars<>, io::no_quote_escape<','>> in(path.u8string());
    std::string file = path.filename().string();

    std::string pickup_label = file[0] == 'g' ? "lpep_pickup_datetime" : "tpep_pickup_datetime";
    std::string plon_label = file[0] == 'g' ? "Pickup_longitude" : "pickup_longitude";
    std::string plat_label = file[0] == 'g' ? "Pickup_latitude" : "pickup_latitude";
    

    in.read_header(io::ignore_extra_column, pickup_label, plon_label, plat_label);
    if (file[0] == 'g'){
        std::string a = std::string(1,file[20]);
        std::string b  = std::string(1,file[21]);
        if (std::stoi(a+b) < 7){
            in.col_order.push_back(-1);
            in.col_order.push_back(-1);
        }
    }
    std::cout << "File: " << file << std::endl;
    
    std::string node_name; float lon; float lat;
    int bad_row = 0;

    while(in.read_row(node_name, lon, lat)){
        if ( lon == 0 || lat == 0)
            bad_row++;
        else{
            box b(point(lon,lat), point(lon,lat));
            rtree.insert(std::make_pair(b, node_name));
        }
    }
    std::cout << bad_row << " incomplete rows" << std::endl;
}