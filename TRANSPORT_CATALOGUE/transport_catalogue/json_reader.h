#pragma once

#include <cstdint>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include "graph.h"
#include "json.h"
#include "json_builder.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"


/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
class TransportRouter;
class InputJSONReader {
public:
    InputJSONReader(std::istream& in) : in_(json::Load(in)) {}
    void ProcessData(TransportCatalogue& catalogue) ;
    void ProcessRenderSettings();
    graph::DirectedWeightedGraph<double> BuildGraph(TransportCatalogue& catalogue);
    std::pair<int, double> GetRoutSettings() const;

    void ProcessStatRequests(std::ostream& out, TransportCatalogue& catalogue, TransportRouter& router);

private:

    int bus_wait_time_ = 0;
    double bus_velosity_ = 0.0;

    json::Document in_;

    std::vector<json::Node> stat_requests_;
    json::Dict render_settings_;
    MapRenderer map_render_;

    std::deque<geo::Coordinates> all_coords_;
    std::map<std::string, std::vector<const geo::Coordinates*>> all_buses_;
    std::map<std::string, std::vector<size_t>> bus_end_stops_indexes_; 
    std::map<std::string, const geo::Coordinates*> stops_coords_;
    
    //std::map<std::string_view, graph::VertexId> stop_to_index_;
    //std::map<graph::VertexId, std::string_view> index_to_stop_;
    //std::unordered_map<graph::EdgeId, std::string_view> edge_id_bus_;

    void ProcessBaseRequests(json::Array base_requests, TransportCatalogue& catalogue) ;

    void ProcessBusRequest(const json::Dict& request, json::Builder& builder, TransportCatalogue& catalogue) const;

    void ProcessStopRequest(const json::Dict& request, json::Builder& builder, TransportCatalogue& catalogue) const;

    void ProcessMapRequest(std::ostream& out) const;

    void ProcessRouteRequest(const json::Dict& request, json::Builder& builder, TransportCatalogue& catalogue, TransportRouter& router);

    static svg::Color SetColor(const json::Node& color_node);

    void Rout(svg::Document& doc, const SphereProjector& spher_proj, const MapSettings* map_sets) const;

    void BusNames(svg::Document& doc, const SphereProjector& spher_proj, const MapSettings* map_sets) const;

    void Stops(svg::Document& doc, const SphereProjector& spher_proj, const MapSettings* map_sets) const;

    void StopNames(svg::Document& doc, const SphereProjector& spher_proj, const MapSettings* map_sets) const;

};