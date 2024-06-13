#include "stat_reader.h"

using namespace std::literals;

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output) {
    // Реализуйте самостоятельно
     
    auto pos = request.find("Bus"sv);
    if (pos == request.npos) {
        return;
    }
    request = request.substr(pos + 3);
    request= request.substr(request.find_first_not_of(' '));
    auto pos1 = request.find_first_of(' ');
    request = request.substr(0, pos1);
    output << "\n"s << tansport_catalogue.GetBusInfo(request);
}