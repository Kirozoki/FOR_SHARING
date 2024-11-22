#include "json_reader.h"

using namespace std::literals;
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

void InputJSONReader::ProcessData(TransportCatalogue& catalogue) {
    
        for (const auto& [key, value] : in_.GetRoot().AsDict()) {
            if (key == "base_requests"s) {
                if (value.IsArray()) {
                    ProcessBaseRequests(value.AsArray(), catalogue);
                }
            } else if (key == "stat_requests"s) {
                if (value.IsArray()) {
                    if (!value.AsArray().empty()) {
                    for (const json::Node& node : value.AsArray()) {
                        if (node.IsDict()) {
                            stat_requests_.emplace_back(node);
                        }
                    }
                    }
                }    
            } else if (key == "render_settings"s) {
                if (value.IsDict() && !value.AsDict().empty()) {
                    render_settings_ = value.AsDict();
                }
            } else if (key == "routing_settings"s) { 
                if (value.IsDict() && !value.AsDict().empty()) {
                    const json::Dict& dict = value.AsDict();
                    bus_wait_time_ = dict.at("bus_wait_time"s).AsInt();
                    
                    if (dict.at("bus_velocity"s).IsDouble()) {
                        bus_velosity_ = dict.at("bus_velocity"s).AsDouble();  
                    } else if (dict.at("bus_velocity"s).IsInt()) {
                        bus_velosity_ = (double)dict.at("bus_velocity"s).AsInt();
                    }
                }
            }
        }

        for (const auto& [key, value] : in_.GetRoot().AsDict()) {
            if (key == "base_requests"s) {
                    for (const json::Node& node : value.AsArray()) {
                        if (node.IsDict() && node.AsDict().at("type"s).AsString() == "Stop"s) {
                            json::Dict stop_data = node.AsDict();
                            if (stop_data.at("road_distances"s).IsDict()) {
                                json::Dict stops = stop_data.at("road_distances"s).AsDict();
                                if (!stops.empty()) {
                                    for (const auto& [stop_name, dist] : stops) {
                                        catalogue.AddDist(stop_data.at("name"s).AsString(), stop_name
                                                            , dist.AsDouble());
                                    }
                                }
                            }
                        }
                    }
            }
        }
}

std::pair<int, double> InputJSONReader::GetRoutSettings() const {
    return {bus_wait_time_, bus_velosity_};
}

void InputJSONReader::ProcessBaseRequests(json::Array base_requests, TransportCatalogue& catalogue) {
    for (const json::Node& node : base_requests) {
        if (node.IsDict() && node.AsDict().at("type"s).AsString() == "Stop"s) {
            json::Dict stop_data = node.AsDict();
            catalogue.AddStop(stop_data.at("name"s).AsString(), 
                        {stop_data.at("latitude"s).AsDouble(), 
                        stop_data.at("longitude"s).AsDouble()});
                        }
    }

    for (const json::Node& node : base_requests) {
        if (node.IsDict() && node.AsDict().at("type"s).AsString() == "Bus"s) {
            json::Dict bus_data = node.AsDict();
            std::vector<std::string_view> rout_stops;
            if (bus_data.at("stops"s).IsArray()) {
                size_t vec_size = bus_data.at("stops"s).AsArray().size();
                rout_stops.reserve(vec_size);
                for (const json::Node& node: bus_data.at("stops"s).AsArray()) {
                    rout_stops.push_back(
                        catalogue.FindStop(node.AsString())->stop_name_);
                }
                std::string bus_name = bus_data.at("name"s).AsString();
                size_t second_end_stop = 0u;    
                if (bus_data.at("is_roundtrip"s).IsBool() && 
                    !bus_data.at("is_roundtrip"s).AsBool()) {
                    second_end_stop = rout_stops.size() - 1;
                    rout_stops.insert(rout_stops.end(), std::next(rout_stops.rbegin()), rout_stops.rend());
                }
                            
                catalogue.AddBus(bus_name, rout_stops);
                const Bus* bus_ptr = catalogue.FindBus(bus_name);
                std::vector<const geo::Coordinates*> coords;

                for (const Stop* stop_ptr : bus_ptr->bus_stops_) {
                    const geo::Coordinates& it = all_coords_.emplace_back(stop_ptr->coordinates_);
                    coords.push_back(&it);
                    stops_coords_.emplace(stop_ptr->stop_name_, &it);
                }
                all_buses_[bus_name] = coords;
                if (!bus_ptr->bus_stops_.empty()) {
                    
                    if (second_end_stop > 0u && (second_end_stop > 0u && bus_ptr->bus_stops_[0] != bus_ptr->bus_stops_[second_end_stop])) {
                        catalogue.AddBusEndStopsIndeces(bus_name, {0u, second_end_stop});
                        bus_end_stops_indexes_[bus_name] = {0u, second_end_stop};
                    } else {
                        catalogue.AddBusEndStopsIndeces(bus_name,{0u});
                        bus_end_stops_indexes_[bus_name] = {0u};
                    }
                }
            }
        }
    }
}

void InputJSONReader::ProcessStatRequests(std::ostream& out, TransportCatalogue& catalogue, TransportRouter& router) {
    json::Builder builder{};
    builder.StartArray();
    for (const json::Node& node : stat_requests_) {
        
        json::Dict request = node.AsDict();
        if (request.at("type"s).AsString() == "Bus"s) {
            ProcessBusRequest(request, builder, catalogue);
            
        } else if (request.at("type"s).AsString() == "Stop"s) {
            ProcessStopRequest(request, builder, catalogue);
            
        } else if (request.at("type"s).AsString() == "Map"s) {        
            std::ostringstream out;
            ProcessMapRequest(out);
            builder.StartDict()
                .Key("map"s).Value(std::string(out.str()))
                .Key("request_id"s).Value(int(request.at("id"s).AsInt()))
            .EndDict();
        } else if (request.at("type"s).AsString() == "Route"s) {

            ProcessRouteRequest(request, builder, catalogue, router);
            
        }
    }
    builder.EndArray();
    json::Print(json::Document(builder.Build()), out);
}

void InputJSONReader::ProcessBusRequest(const json::Dict& request, json::Builder& builder, TransportCatalogue& catalogue) const {
    BusInfo bus_stat = catalogue.GetBusInfo(request.at("name"s).AsString());
    
    if (bus_stat.name == ""s) {
        builder.StartDict()
            .Key("request_id").Value(int(request.at("id"s).AsInt()))
            .Key("error_message").Value("not found")
        .EndDict();
    } else {
        builder.StartDict()
            .Key("curvature").Value(double(bus_stat.curvature))
            .Key("request_id").Value(int(request.at("id"s).AsInt()))
            .Key("route_length").Value(double(bus_stat.routh_length))
            .Key("stop_count").Value(int(bus_stat.stops_qty))
            .Key("unique_stop_count").Value(int(bus_stat.unique_stops_qty))
        .EndDict();
    }
}

void InputJSONReader::ProcessStopRequest(const json::Dict& request, json::Builder& builder, TransportCatalogue& catalogue) const {
    
    StopInfo stop_info = catalogue.GetStopInfo(request.at("name").AsString());
    if (stop_info.name_ == ""s) {
        builder.StartDict()
            .Key("request_id").Value(int(request.at("id"s).AsInt()))
            .Key("error_message").Value(std::string("not found"))
        .EndDict();
    } else {
        builder.StartDict().Key("buses").StartArray();
        for(const std::string_view& bus_name : stop_info.bus_names_) {
            builder.Value(std::string(bus_name));
        }
            builder.EndArray()
            .Key("request_id").Value(int(request.at("id"s).AsInt()))
        .EndDict();
    }    
}

void InputJSONReader::ProcessMapRequest(std::ostream& out) const {
        if (stat_requests_.empty()) {
            return;
        }
        svg::Document doc;
        const MapSettings* map_sets = map_render_.GetSettings();
        SphereProjector spher_proj(all_coords_.begin(), all_coords_.end(),
                map_sets->width, map_sets->height, map_sets->padding);
        map_render_.RoutRender(doc, spher_proj, all_buses_);
        map_render_.BusNamesRender(doc, spher_proj, all_buses_, bus_end_stops_indexes_);
        map_render_.StopsRender(doc, spher_proj, stops_coords_);
        map_render_.StopNamesRender(doc, spher_proj, stops_coords_);
        doc.Render(out);
}

void InputJSONReader::ProcessRenderSettings() {
    if (render_settings_.empty()) {
        return;
    }
    MapSettings map_settings;
    map_settings.width = render_settings_.at("width"s).AsDouble();
    map_settings.height = render_settings_.at("height"s).AsDouble();
    map_settings.padding = render_settings_.at("padding"s).AsDouble();
    map_settings.line_width = render_settings_.at("line_width"s).AsDouble();
    map_settings.stop_radius = render_settings_.at("stop_radius"s).AsDouble();
    map_settings.bus_label_font_size = render_settings_.at("bus_label_font_size"s).AsInt();
    
    map_settings.bus_label_offset.x = render_settings_.at("bus_label_offset"s).AsArray()[0].AsDouble();
    map_settings.bus_label_offset.y = render_settings_.at("bus_label_offset"s).AsArray()[1].AsDouble();
    
    map_settings.stop_label_font_size = render_settings_.at("stop_label_font_size"s).AsInt();
    
    map_settings.stop_label_offset.x = render_settings_.at("stop_label_offset"s).AsArray()[0].AsDouble();
    map_settings.stop_label_offset.y = render_settings_.at("stop_label_offset"s).AsArray()[1].AsDouble();

    json::Node underlayer_color = render_settings_.at("underlayer_color"s);
    map_settings.underlayer_color = SetColor(underlayer_color);
    map_settings.underlayer_width = render_settings_.at("underlayer_width"s).AsDouble();

    json::Array collor_pallete = render_settings_.at("color_palette"s).AsArray();
    for (const json::Node& color_node : collor_pallete) {
        map_settings.color_palette.emplace_back(SetColor(color_node));
    }

    map_render_.SetSettings(map_settings);   
}

void InputJSONReader::ProcessRouteRequest(const json::Dict& request, json::Builder& builder, TransportCatalogue& catalogue
                                            , TransportRouter& router) {
    std::string_view from = request.at("from"s).AsString();
    std::string_view to = request.at("to"s).AsString();
    StopInfo from_info = catalogue.GetStopInfo(from);
    StopInfo to_info = catalogue.GetStopInfo(to);
    if (bus_velosity_ != 0 && !from_info.bus_names_.empty()
            && !to_info.bus_names_.empty()) {
        
        std::optional<graph::Router<double>::RouteInfo> rout_info = 
                                        router.FindRoute(from, to);
        if (rout_info) {
            
            std::vector<graph::EdgeId>& edges_ids = rout_info.value().edges;
            double total_weight = 0.0;
            builder.StartDict()
                .Key("request_id"s).Value(int(request.at("id"s).AsInt()))
                .Key("items"s).StartArray();
            for (size_t i = 0; i < edges_ids.size(); ++i) {
                const graph::Edge<double> edge = router.GetEdge(edges_ids[i]);
                
                graph::VertexId stop_from_id = edge.from; // first
                
                    if (edge.span == 0) {
                    builder.StartDict()
                        .Key("stop_name"s).Value(std::string(router.GetStopName(stop_from_id)))
                        .Key("time"s).Value(int(bus_wait_time_))
                        .Key("type"s).Value(std::string("Wait"s))
                        .EndDict();
                    total_weight += (double)bus_wait_time_;
                    } else {
                
                    std::string_view route_name = edge.span == 0 ? ""sv : router.GetRouteName(edges_ids[i]);
                    builder.StartDict()
                        .Key("bus"s).Value(std::string(route_name))
                        .Key("span_count"s).Value(int(edge.span))
                        .Key("time"s).Value(double(edge.weight))
                        .Key("type"s).Value(std::string("Bus"s))
                        .EndDict();              
                
                        total_weight += edge.weight;
                    }
            }
            builder.EndArray()
                .Key("total_time"s).Value(double(total_weight))
                .EndDict();

        } else {
            builder.StartDict()
                .Key("request_id").Value(int(request.at("id"s).AsInt()))
                .Key("error_message").Value(std::string("not found"s))
            .EndDict();
        }
    } else {
        builder.StartDict()
            .Key("request_id").Value(int(request.at("id"s).AsInt()))
            .Key("error_message").Value(std::string("not found"s))
        .EndDict();
    }
}

svg::Color InputJSONReader::SetColor(const json::Node& color_node) {
    svg::Color color;
    if (color_node.IsString()) {
        color = color_node.AsString();
    } else if (color_node.IsArray()) {
        if (color_node.AsArray().size() == 3) {
            color = svg::Rgb{static_cast<uint8_t>(color_node.AsArray()[0].AsInt()), 
                    static_cast<uint8_t>(color_node.AsArray()[1].AsInt()), 
                    static_cast<uint8_t>(color_node.AsArray()[2].AsInt())};
        } else if (color_node.AsArray().size() == 4) {
            color = svg::Rgba{static_cast<uint8_t>(color_node.AsArray()[0].AsInt()), 
                    static_cast<uint8_t>(color_node.AsArray()[1].AsInt()), 
                    static_cast<uint8_t>(color_node.AsArray()[2].AsInt()),
                    color_node.AsArray()[3].AsDouble()};
        }
    }
    return color;
} 

void InputJSONReader::Rout(svg::Document& doc, const SphereProjector& spher_proj,
                                const MapSettings* map_sets) const {
        
        size_t colors_qty = map_sets->color_palette.size();
        size_t color_index = 0;

        for (const auto& [bus_name, vec_coords] : all_buses_) {
            if (vec_coords.empty()) {
                continue;
            }
            svg::Polyline poly;
            for (size_t i = 0; i < vec_coords.size(); ++i) {
                svg::Point point = spher_proj(*vec_coords[i]);
                poly.AddPoint(point);
            }
            poly.SetFillColor(svg::NoneColor).SetStrokeColor(map_sets->color_palette[color_index]).
                SetStrokeWidth(map_sets->line_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND).
                SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            doc.Add(poly);
            color_index = color_index + 1 == colors_qty ? 0 :
                            color_index + 1;
        }
}

void InputJSONReader::BusNames(svg::Document& doc, const SphereProjector& spher_proj, 
                                    const MapSettings* map_sets) const {
    
    size_t colors_qty = map_sets->color_palette.size();
    size_t color_index = 0;
    for (const auto& [bus_name, vec_stops] : bus_end_stops_indexes_) {
            if (vec_stops.empty()) {continue;}
            for (const size_t& stop_ind : vec_stops) {
                svg::Point stop_p = spher_proj(*all_buses_.at(bus_name)[stop_ind]);

                const svg::Text text = svg::Text().SetFontFamily("Verdana")
                    .SetFontSize(map_sets->bus_label_font_size)
                    .SetFontWeight("bold")
                    .SetPosition({stop_p.x, stop_p.y})
                    .SetData(bus_name)
                    .SetOffset(map_sets->bus_label_offset);
                doc.Add(svg::Text{text}
                            .SetStrokeColor(map_sets->underlayer_color)
                            .SetFillColor(map_sets->underlayer_color)
                            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                            .SetStrokeWidth(map_sets->underlayer_width));
                doc.Add(svg::Text{text}
                            .SetFillColor(map_sets->color_palette[color_index]));
            }
            color_index = color_index + 1 == colors_qty ? 0 :
                            color_index + 1;
        }
}

void InputJSONReader::Stops(svg::Document& doc, const SphereProjector& spher_proj, 
                                const MapSettings* map_sets) const {
    for(const auto& [stop_name, coords] : stops_coords_) {
        svg::Point circle_center = spher_proj(*coords);
        svg::Circle circle = svg::Circle().SetCenter(circle_center)
                                            .SetRadius(map_sets->stop_radius);
        doc.Add(svg::Circle{circle}
                            .SetFillColor(svg::Color{"white"}));
    }
}

void InputJSONReader::StopNames(svg::Document& doc, const SphereProjector& spher_proj, 
                                    const MapSettings* map_sets) const {
    for (const auto& [stop_name, coord_ptr] : stops_coords_) {
        svg::Point stop_p = spher_proj(*coord_ptr);

                const svg::Text text = svg::Text().SetFontFamily("Verdana")
                    .SetFontSize(map_sets->stop_label_font_size)
                    .SetPosition({stop_p.x, stop_p.y})
                    .SetData(stop_name)
                    .SetOffset(map_sets->stop_label_offset);
                doc.Add(svg::Text{text}
                            .SetStrokeColor(map_sets->underlayer_color)
                            .SetFillColor(map_sets->underlayer_color)
                            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                            .SetStrokeWidth(map_sets->underlayer_width));
                doc.Add(svg::Text{text}
                            .SetFillColor(svg::Color{"black"}));
    }
}