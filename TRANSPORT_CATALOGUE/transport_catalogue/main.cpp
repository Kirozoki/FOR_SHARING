#include <iostream>
#include <string>
#include <fstream> // del

#include "json_reader.h"

using namespace std::literals;

int main() {
    
    std::ifstream in("in4.json"s);
    if (!in) {return 0;}
    std::ofstream out("out4.json"s);

    std::string full_string;
    std::string str;
    while (std::getline(in, str)){ // cin
        if (str == "exit"s) {break;}
        full_string += str;
    }
    std::istringstream input(full_string);
    std::ostringstream output;
    TransportCatalogue catalogue;  
    InputJSONReader reader(input);
    reader.ProcessData(catalogue);
    reader.ProcessRenderSettings();

    auto [wait_time, bus_velosity] = reader.GetRoutSettings();
    TransportRouter router{wait_time, bus_velosity, catalogue};

    reader.ProcessStatRequests(output, catalogue, router);
    
    out << output.str() << std::endl; // cout
}