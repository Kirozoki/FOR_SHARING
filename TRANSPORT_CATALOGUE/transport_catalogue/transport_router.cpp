// Вставьте сюда решение из предыдущего спринта
#include "transport_router.h"

TransportRouter::TransportRouter(int bus_wait_time, double bus_velocity, const TransportCatalogue& catalogue) : 
rout_info_({bus_wait_time, bus_velocity}), catalogue_(catalogue)
    {
        BuildGraph();
        router_ = std::make_unique<graph::Router<double>>(graph_);        
    }

const std::optional<graph::Router<double>::RouteInfo> TransportRouter::FindRoute
        (std::string_view from, std::string_view to) const {
    return router_->BuildRoute(stop_to_index_.at(from), stop_to_index_.at(to));
}

std::string_view TransportRouter::GetRouteName(graph::EdgeId edge_id) const {
    return edge_id_bus_.at(edge_id);
}
std::string_view TransportRouter::GetStopName(graph::VertexId vertex_id) const {
    return index_to_stop_.at(vertex_id);
}

graph::Edge<double> TransportRouter::GetEdge(graph::EdgeId edge_id) const {
    return graph_.GetEdge(edge_id);
}

void TransportRouter::BuildGraph() {
    std::map<std::string_view, std::vector<const Stop*>> all_buses = catalogue_.GetAllBuses();
    std::map<std::string_view, const Stop*> all_stops = catalogue_.GetAllStops();
    graph::DirectedWeightedGraph<double> graph(all_stops.size()); // updated with x1 time for stops at least
    AddStopsToGraph(graph, all_stops);
    AddRoutesToGraph(graph, all_buses);
    graph_ = std::move(graph);
}

void TransportRouter::AddStopsToGraph(graph::DirectedWeightedGraph<double>& graph, 
                                const std::map<std::string_view, const Stop*>& all_stops) {
    size_t vertex_id = 0;
    for (const auto& [stop_name, stop_ptr]: all_stops) {
        stop_to_index_[stop_name] = {vertex_id}; // first
        index_to_stop_[vertex_id] = stop_name; // first
        graph.AddEdge(graph::Edge<double>{vertex_id, ++vertex_id, (double)rout_info_.bus_wait_time_, 0});
        ++vertex_id;      
    }
}

void TransportRouter::AddRoutesToGraph(graph::DirectedWeightedGraph<double>& graph, 
                const std::map<std::string_view, std::vector<const Stop*>>& all_buses) {
    
    const double bus_velocity = rout_info_.bus_velocity_ * 100.0 / 6.0; // m/min
    for (const auto& [bus_name, stops_v] : all_buses) {
        if (stops_v.empty()) {continue;}
        std::vector<size_t> end_stops_indeces = catalogue_.GetEndStopsIndeces(bus_name);
        
        bool is_round_trip = end_stops_indeces.size() == 1;
        
        size_t end_index = (is_round_trip) ? stops_v.size() : end_stops_indeces[1] + 1;
        for (size_t i = 0; i + 1 < end_index; ++i) {
            double edge_dist = 0.0;
            double edge_dist_reverse = 0.0;
            for (size_t j = i + 1; j < end_index; ++j) {
                const Stop* stop_prev = stops_v[j -1];
                const Stop* stop_to = stops_v[j];
                const Stop* stop_from = stops_v[i];
                edge_dist += (double) catalogue_.GetDist( stop_prev,
                                                    stop_to);
                
                graph::VertexId from_id = stop_to_index_.at(stop_from->stop_name_) + 1;
                graph::VertexId to_id = stop_to_index_.at(stop_to->stop_name_);
                graph::EdgeId edge_id = graph.AddEdge(graph::Edge<double>(from_id, to_id, edge_dist / bus_velocity, j-i)); 
                edge_id_bus_[edge_id] = bus_name;

                if (!is_round_trip) {
                    edge_dist_reverse += (double) catalogue_.GetDist(stop_to, stop_prev);
                    graph::EdgeId edge_id = graph.AddEdge(graph::Edge<double>(to_id + 1, from_id - 1, edge_dist_reverse / bus_velocity, j-i)); 
                    edge_id_bus_[edge_id] = bus_name;
                }
            } 
        }
    }
}