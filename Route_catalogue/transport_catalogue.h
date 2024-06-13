#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "geo.h"

class TransportCatalogue {
	// Реализуйте класс самостоятельно
	public:
	struct Stop {
		std::string stop_name_;
		Coordinates coordinates_;
		bool operator==(const Stop& other) const {
			return stop_name_ == other.stop_name_ &&
					coordinates_ == other.coordinates_;
		}
		
		bool operator!=(const Stop& other) const {
			return !(*this == other);
		}
	};

	struct Bus {
		std::string bus_name_;
		std::vector<const Stop*> bus_stops_;
	};
	
	void AddStop(std::string stop_id, Coordinates coord);  
	void AddBus(std::string bus_name, std::vector<std::string_view> stops);
	const Stop* FindStop(std::string_view stop_name) const;
	const Bus* FindBus(std::string_view bus_name) const;
	std::string_view GetBusInfo(std::string_view bus_name) const;

	private:
	std::deque<Stop> stops_;
	std::unordered_map<std::string_view, const Stop*> names_to_stops_;
	std::deque<Bus> buses_;
	std::unordered_map<std::string_view, const Bus*> names_to_buses_;
	//std::unordered_map<std::pair<Stop*, Stop*>, double, DistHasher> distances_;

};
