// Вставьте сюда решение из предыдущего спринта
#include "transport_catalogue.h"

using namespace std::literals;

void TransportCatalogue::AddStop(const std::string& stop_id, geo::Coordinates coord) {
    
    Stop& ref = stops_.emplace_back(Stop{stop_id, coord});
    Stop* ptr = &ref;
    names_to_stops_[ptr->stop_name_] = ptr;
}

void TransportCatalogue::AddDist(const std::string& from, const std::string& to, double dist) {
    
    const Stop* stop_from = FindStop(from);
    const Stop* stop_to = FindStop(to);
    if (!stop_from || !stop_to) {return;}

    if (distances_.find({stop_from, stop_to}) == distances_.end()) {
        distances_[{stop_to, stop_from}] = dist;
    }
    distances_[{stop_from, stop_to}] = dist;
    
}

void TransportCatalogue::AddBus(const std::string& bus, const std::vector<std::string_view>& stops) {
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
        stop_to_buses_[stop_ptr].insert(ptr->bus_name_);
    }
}

const Stop* TransportCatalogue::FindStop(std::string_view stop_name) const {
    const auto stop_it = names_to_stops_.find(stop_name);
    if (stop_it == names_to_stops_.end()) {
        return nullptr;
    }
    return stop_it->second;
}
const Bus* TransportCatalogue::FindBus(std::string_view bus_name) const {
    const auto bus_it = names_to_buses_.find(bus_name);
    if (bus_it == names_to_buses_.end()) {
        return nullptr;
    }
    return bus_it->second;
}

BusInfo TransportCatalogue::GetBusInfo(std::string_view bus_name) const {
    //Bus X: R stops on route, U unique stops, L route length, C curvature
    //Bus X: not found
    const Bus* bus_ptr = FindBus(bus_name);
    if (!bus_ptr) {
        return BusInfo{""s, 0, 0, 0.0, 0.0}; 
    }
    
    int stops_qty = bus_ptr->bus_stops_.size();
    std::unordered_set<const Stop*> unique_set{bus_ptr->bus_stops_.begin(), bus_ptr->bus_stops_.end()};
    int unique_stops_qty = unique_set.size();
    double geo_routh_length = 0.0;
    double routh_length = 0.0;
    for (int i = 1; i < stops_qty; ++i) {
        geo_routh_length += geo::ComputeDistance(bus_ptr->bus_stops_.at(i)->coordinates_, 
                                        bus_ptr->bus_stops_.at(i-1)->coordinates_);
        routh_length += GetDist(bus_ptr->bus_stops_.at(i-1), 
                                bus_ptr->bus_stops_.at(i));
    }
    double curvature = routh_length / geo_routh_length;
    return BusInfo{bus_name, stops_qty, unique_stops_qty, routh_length, curvature};
}

StopInfo TransportCatalogue::GetStopInfo(std::string_view stop_name) const {
    const Stop* stop_ptr = FindStop(stop_name);
    if (!stop_ptr) {
        return StopInfo{""sv, {}}; 
    }
    const auto stop_iter = stop_to_buses_.find(stop_ptr) ;
    if (stop_iter == stop_to_buses_.end()) {
        return StopInfo{stop_name, {}}; 
    }
    return StopInfo{stop_name, stop_iter->second};
}

int TransportCatalogue::GetDist(const Stop* stop_from, const Stop* stop_to) const {
    
    const auto stops_iter = distances_.find({stop_from, stop_to});
    if (stops_iter == distances_.end()) {
        return 0;
    }
    return stops_iter->second;
}

std::map<std::string_view, std::vector<const Stop*>> TransportCatalogue::GetAllBuses() const {
    std::map<std::string_view, std::vector<const Stop*>> result;
    for (const auto& [bus_name, bus_ptr] : names_to_buses_) {
        result[bus_name] = bus_ptr->bus_stops_;
    }
    return result;
}

std::map<std::string_view, const Stop*> TransportCatalogue::GetAllStops() const {
    return {names_to_stops_.begin(), names_to_stops_.end()};
}

void TransportCatalogue::AddBusEndStopsIndeces(std::string_view bus_name, std::vector<size_t> end_stops_indeces) {
    const Bus* bus_ptr = FindBus(bus_name);
    
    if (bus_ptr) {
    bus_end_stops_indeces_[bus_ptr->bus_name_] = end_stops_indeces;
    }
}
std::vector<size_t> TransportCatalogue::GetEndStopsIndeces(std::string_view bus_name) const {
    if (!FindBus(bus_name)) {
        return {};
    }
    const auto it = bus_end_stops_indeces_.find(bus_name);
    if (it == bus_end_stops_indeces_.end()) {
        return {};
    }
    return bus_end_stops_indeces_.at(bus_name);
}