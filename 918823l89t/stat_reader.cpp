#include "stat_reader.h"

using namespace std::literals;

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output) {
    // Реализуйте самостоятельно
     
    auto pos = request.find("Bus"sv);
    if (pos != request.npos) {
        request = request.substr(pos + 3);
        request= request.substr(request.find_first_not_of(' '));
        auto pos1 = request.find_last_not_of(' ');
        pos1 = pos1 == request.size() - 1 ? request.npos : pos1 + 1;
        request = request.substr(0, pos1);
        output << tansport_catalogue.GetBusInfo(request) << std::endl;
    } else if (request.find("Stop"sv) != request.npos) {
        pos = request.find("Stop"sv);
        request = request.substr(pos + 4);
        request= request.substr(request.find_first_not_of(' '));
        auto pos1 = request.find_last_not_of(' ');
        pos1 = pos1 == request.size() - 1 ? request.npos : pos1 + 1;
        request = request.substr(0, pos1);
        output << tansport_catalogue.GetStopInfo(request) << std::endl;
    }

}