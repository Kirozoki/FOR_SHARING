#pragma once
#include "json_reader.h"
#include "transport_catalogue.h"
#include "router.h"

class TransportRouter {
    public:
    TransportRouter() = default;

    explicit TransportRouter(int bus_wait_time, double bus_velocity, const TransportCatalogue& catalogue);
    
    ~TransportRouter() = default;

    const std::optional<graph::Router<double>::RouteInfo> FindRoute
        (std::string_view from, std::string_view to) const;

    std::string_view GetRouteName(graph::EdgeId edge_id) const; // public for json_reader 
    std::string_view GetStopName(graph::VertexId vertex_id) const; // public for json reader
    graph::Edge<double> GetEdge(graph::EdgeId edge_id) const; 
    
    private:
    struct RouteInfo {
        int bus_wait_time_ = 0;
        double bus_velocity_ = 0.0;
    };

    RouteInfo rout_info_{0, 0.0};
    
    std::unique_ptr<graph::Router<double>> router_;
    graph::DirectedWeightedGraph<double> graph_;
    const TransportCatalogue& catalogue_;

    std::map<std::string_view, graph::VertexId> stop_to_index_;
    std::map<graph::VertexId, std::string_view> index_to_stop_;
    std::unordered_map<graph::EdgeId, std::string_view> edge_id_bus_;

    void BuildGraph();
    void AddStopsToGraph(graph::DirectedWeightedGraph<double>& graph, 
                            const std::map<std::string_view, const Stop*>& all_stops);
    void AddRoutesToGraph(graph::DirectedWeightedGraph<double>& graph, 
                            const std::map<std::string_view, std::vector<const Stop*>>& all_buses);
};