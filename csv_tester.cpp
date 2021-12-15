
#include "./2015-data/csv-parser/csv.hpp"
#include "./2015-data/csv-parser/rapidcsv.h"
#include "./2015-data/csv-parser/fast.h"
#include <chrono>

int main(){
    std::string path = "../2015-data/data/yellow_tripdata_2015-01.csv";
    std::string pickup_label = "tpep_pickup_datetime";
    std::string plon_label = "pickup_longitude";
    std::string plat_label = "pickup_latitude";

    std::cout << "yellow_tripdata_2015-01.csv Read Test\n" << std::endl;

    // FAST

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    io::CSVReader<3,io::trim_chars<>, io::no_quote_escape<','>> in(path);
    in.read_header(io::ignore_extra_column, pickup_label, plon_label, plat_label);
    std::string node_name; float lon; float lat;
    int rows = 0;
    while(in.read_row(node_name, lon, lat)){
        rows++;
    }

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "FAST: " << time_span.count() << " seconds." << std::endl;

    // VINCE

    std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();

    using namespace csv;
    CSVReader reader(path);
    int rows1 = 0;
    for (CSVRow& row: reader){
        rows1++;
    }

    std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span1 = std::chrono::duration_cast<std::chrono::duration<double>>(t4 - t3);
    std::cout << "VINCE: " << time_span1.count() << " seconds." << std::endl;

    // RAPID

    std::chrono::high_resolution_clock::time_point t5 = std::chrono::high_resolution_clock::now();

    rapidcsv::Document doc(path);
    int rows2 = 0;
    for (long unsigned int i = 0; i < doc.GetRowCount(); ++i){
        rows2++;
    }

    std::chrono::high_resolution_clock::time_point t6 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span2 = std::chrono::duration_cast<std::chrono::duration<double>>(t6 - t5);
    std::cout << "RAPID: " << time_span2.count() << " seconds." << std::endl;
}