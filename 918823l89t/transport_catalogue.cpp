#include "transport_catalogue.h"

using namespace std::literals;

void TransportCatalogue::AddStop(std::string stop_id, Coordinates coord) {
    
    Stop& ref = stops_.emplace_back(Stop{stop_id, coord});
    Stop* ptr = &ref;
    names_to_stops_[ptr->stop_name_] = ptr;
}

void TransportCatalogue::AddBus(std::string bus, std::vector<std::string_view> stops) {
    if (FindBus(bus)) {
        return;
    }
    std::vector<const Stop*> bus_stops;
    
    for (const std::string_view& stop_name : stops) {
        const Stop* cur_ptr = FindStop(std::string{stop_name});
        if (!cur_ptr) {
            return;
        }
        bus_stops.push_back(cur_ptr);
    }    
    Bus& ref = buses_.emplace_back(Bus{bus, bus_stops});
    Bus* ptr = &ref;
    names_to_buses_[ptr->bus_name_] = ptr;
    for (const Stop* stop_ptr : ptr->bus_stops_) {
        stop_to_buses_[stop_ptr].insert(ptr);
    }
}

const TransportCatalogue::Stop* TransportCatalogue::FindStop(std::string_view stop_name) const {
if (names_to_stops_.find(stop_name) == names_to_stops_.end()) {
    return nullptr;
}
return names_to_stops_.at(stop_name);
}
const TransportCatalogue::Bus* TransportCatalogue::FindBus(std::string_view bus_name) const {
    if (names_to_buses_.find(bus_name) == names_to_buses_.end()) {
    return nullptr;
}
return names_to_buses_.at(bus_name);
}

std::string TransportCatalogue::GetBusInfo(std::string_view bus_name) const {
    //Bus X: R stops on route, U unique stops, L route length
    //Bus X: not found
    std::ostringstream out;
    if (!FindBus(bus_name)) {
        out << "Bus "s << bus_name << ": not found"s;
        return out.str(); 
    }
    const Bus* bus_ptr = FindBus(bus_name);
    int stops_qty = bus_ptr->bus_stops_.size();
    std::unordered_set<const Stop*> unique_set{bus_ptr->bus_stops_.begin(), bus_ptr->bus_stops_.end()};
    int unique_stops_qty = unique_set.size();
    double routh_length = 0.0;
    for (int i = 1; i < stops_qty; ++i) {
        routh_length += ComputeDistance(bus_ptr->bus_stops_.at(i)->coordinates_, 
                                        bus_ptr->bus_stops_.at(i-1)->coordinates_);
    }
    out << "Bus "s << bus_name << ": "s << stops_qty << " stops on route, "s <<
        unique_stops_qty << " unique stops, "s << routh_length << " route length"s;
    return out.str();
}

std::string TransportCatalogue::GetStopInfo(std::string_view stop_name) const {
    std::ostringstream out;
    out << "Stop "s << stop_name << ": "s;
    if (!FindStop(stop_name)) {
        out << "not found"s;
        return out.str(); 
    }
    const Stop* stop_ptr = FindStop(stop_name);

    if (stop_to_buses_.count(stop_ptr) == 0) {
        out << "no buses"s;
        return out.str(); 
    }
    out << "buses"s;
    for (const Bus* bus : stop_to_buses_.at(stop_ptr)) {
        out << " "s<< bus->bus_name_;
    }
    return out.str();
}
