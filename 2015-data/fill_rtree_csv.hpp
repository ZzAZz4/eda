#include "./csv-parser/csv.hpp"
#include <iostream>
#include <string>
#include <vector>
using namespace csv;

/* if __cpp_lib_experimental_filesystem */
0
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

/* if __cpp_lib_filesystem */

// #include <filesystem>
// namespace fs = filesystem;


static std::vector<fs::path> files_in_folder(fs::path path){
	std::vector<fs::path> fileNamePaths;
	if (!exists(path))
		return fileNamePaths;
	
	for (const auto& entry : fs::directory_iterator(path))
		// fileNamePaths.push_back(fs::path(entry).filename());
		fileNamePaths.push_back(entry);
	
	return fileNamePaths;
}

// fhv tripdata: IGNORE
// green tripdata:
	// node name: lpep_pickup_datetime
	// y: Pickup_longitude
	// x: Pickup_latitude
// yellow tripdata:
	// node name: tpep_pickup_datetime
	// y: pickup_longitude
	// x: pickup_latitude

bool rtree_filler(){
	std::vector<fs::path>  vec = files_in_folder("./data/");

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
				std::cout << "fhv csv ignored\n";
			break;
			case 'g':
				std::cout << "green csv\n";
				for (auto& row: reader){
					auto node_name = row["lpep_pickup_datetime"].get<std::string>();
					auto y = row["Pickup_longitude"].get<double>();
					auto x = row["Pickup_latitude"].get<double>();

					std::cout << node_name << " - " << y << " - " << x <<"\n";
				}
				std::cout << "file finished\n";

			break;

			case 'y':
				std::cout << "yellow csv\n";
				for (auto& row: reader){
					auto node_name = row["tpep_pickup_datetime"].get<std::string>();
					auto y = row["pickup_longitude"].get<double>();
					auto x = row["pickup_latitude"].get<double>();

					std::cout << node_name << " - " << y << " - " << x <<"\n";
				}
				std::cout << "file finished\n";
			break;
		}
	}
    return 0;
}
