#include "../index/rtree.hpp"
#include "./csv-parser/csv.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace csv;

using point_type = geom::Point<double, 2>;
using box_type = geom::Box<point_type>;
using index_type = index::RTree<std::string, box_type, 3>;

// (there are fancier ways to do this automatically with 
// preprocessor directives but for now this will do)

/* if __cpp_lib_experimental_filesystem */

// #include <experimental/filesystem>
// namespace fs = std::experimental::filesystem;

/* if __cpp_lib_filesystem */

#include <filesystem>
namespace fs = std::filesystem;

// Outputs a vector of file directories in a select path 
static std::vector<fs::path> files_in_folder(fs::path path){
	std::vector<fs::path> fileNamePaths;
	if (!exists(path))
		return fileNamePaths;
	
	for (const auto& entry : fs::directory_iterator(path))
		fileNamePaths.push_back(entry);
	
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

bool rtree_filler(index_type tree){
	std::cout << "Current path is " << fs::current_path() << '\n';
	std::vector<fs::path>  vec = files_in_folder("../2015-data/data/");
	

	for (fs::path entry : vec){
		CSVFormat format;
		format.header_row(0);
		format.delimiter(',');
		format.variable_columns(true);
		CSVReader reader(entry.u8string(), format);
		std::string file = fs::path(entry).filename();
		char t = file[0];
		switch (t){
			case 'f':
			{
				std::cout << "fhv csv ignored\n";
			}
			break;
			case 'g':
			{
				int cnt = 0;
				std::cout << "green csv\n";
				for (auto& row: reader){
					std::string node_name = row["lpep_pickup_datetime"].get<std::string>();
					double y = row["Pickup_longitude"].get<double>();
					double x = row["Pickup_latitude"].get<double>();

					point_type point{y,x};
					box_type box{point, point};
					tree.insert(box, node_name);
					cnt++;

					// std::cout << node_name << " - " << y << " - " << x <<"\n";
				}
				std::cout << "file finished\n";
			}
			break;
			case 'y':
			{	
				std::cout << "yellow csv\n";
				for (auto& row: reader){
					std::string node_name = row["tpep_pickup_datetime"].get<std::string>();
					double y = row["pickup_longitude"].get<double>();
					double x = row["pickup_latitude"].get<double>();

					point_type point{y,x};
					box_type box{point, point};
					tree.insert(box, node_name);

					// std::cout << node_name << " - " << y << " - " << x <<"\n";
				}
				std::cout << "file finished\n";
			}
			break;
		}
	}
    return 1;
}
