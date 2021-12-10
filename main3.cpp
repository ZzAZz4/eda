    #include <iostream>
    #include <vector>
    #include <string>
    // #include "./2015-data/csv-parser/rapidcsv.h"
    #include "./2015-data/csv-parser/fast.h"
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

    int main()
    {
      std::vector<fs::path> vec = files_in_folder("../2015-data/data/");

      for (const fs::path& entry : vec) {
        std::cout << "Open " << entry.u8string() << std::endl;

        // rapidcsv::Document doc(entry.u8string());
        
        io::CSVReader<3> in(entry.u8string());
        in.read_header(io::ignore_extra_column, "tpep_pickup_datetime", "pickup_longitude", "pickup_latitude");
        std::string name; double lon; double lat;
        int bad_row = 0;
        while(in.read_row(name, lon, lat)){
          if ( lon == 0 || lat == 0)
            bad_row++;
          // else std::cout << name << ", " << lon << ", " << lat << "\n";
          // lon = lat = 99999;
          // do stuff with the data
        }
        std::cout << bad_row << " incomplete rows" << std::endl;

        // for (long unsigned int i = 0; i < doc.GetRowCount(); ++i){
        //   std::cout << doc.GetCell<std::string>("tpep_pickup_datetime",i) << ", " << doc.GetCell<std::string>("pickup_longitude",i) << ", " << doc.GetCell<std::string>("pickup_latitude",i) << "\n";
        // }    
        // std::cout << "Read " << doc.GetRowCount() << " values." << std::endl;
      }
    }