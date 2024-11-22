// Вставьте сюда решение из предыдущего спринта
#pragma once

#include <deque>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "geo.h"

struct Stop {
	std::string stop_name_;
	geo::Coordinates coordinates_;
	bool operator==(const Stop& other) const {
		return stop_name_ == other.stop_name_ &&
					coordinates_ == other.coordinates_;
	}
		
	bool operator!=(const Stop& other) const {
		return !(*this == other);
		}
	};

struct StopInfo {
	std::string_view name_;
	std::set<std::string_view> bus_names_;
};

struct Bus {
	std::string bus_name_;
	std::vector<const Stop*> bus_stops_;
};

struct BusInfo {
	std::string_view name;
	int stops_qty;
	int unique_stops_qty;
	double routh_length;
	double curvature;
};

class TransportCatalogue {
	// Реализуйте класс самостоятельно
	public:
	
	
	void AddStop(const std::string& stop_id, geo::Coordinates coord);  
	void AddDist(const std::string& from, const std::string& to, double dist);

	void AddBus(const std::string& bus_name, const std::vector<std::string_view>& stops);

	const Stop* FindStop(std::string_view stop_name) const;
	const Bus* FindBus(std::string_view bus_name) const; 
	
	BusInfo GetBusInfo(std::string_view bus_name) const;
	StopInfo GetStopInfo(std::string_view bus_name) const;
	int GetDist(const Stop* stop_from, const Stop* stop_to) const; 

	std::map<std::string_view, std::vector<const Stop*>> GetAllBuses() const;// added
	std::map<std::string_view, const Stop*> GetAllStops() const; // added
	void AddBusEndStopsIndeces(std::string_view bus_name, std::vector<size_t> end_stops_indeces); // added
	std::vector<size_t> GetEndStopsIndeces(std::string_view bus_name) const; // added

	struct DistHasher{
		size_t operator() (const std::pair<const Stop*, const Stop*>& el) const {
			return ptr_hasher_(el.first) + ptr_hasher_(el.second) * 37;
		} 
		private:
		std::hash<const void*> ptr_hasher_;
	};

	private:
	std::deque<Stop> stops_;
	std::unordered_map<std::string_view, const Stop*> names_to_stops_;
	std::deque<Bus> buses_;
	std::unordered_map<std::string_view, const Bus*> names_to_buses_;
	std::unordered_map<const Stop*, std::set<std::string_view>> stop_to_buses_;
	std::unordered_map<std::pair<const Stop*, const Stop*>, int, DistHasher> distances_;

	std::unordered_map<std::string_view, std::vector<size_t>> bus_end_stops_indeces_;
};
