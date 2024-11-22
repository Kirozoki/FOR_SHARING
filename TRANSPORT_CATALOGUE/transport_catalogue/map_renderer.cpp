// Вставьте сюда решение из предыдущего спринта
#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
}

MapSettings& MapSettings::operator=(const MapSettings& r) {
    width = r.width; 
    height = r.height; 
    padding = r.padding; 
    line_width = r.line_width; 
    stop_radius = r.stop_radius; 
    bus_label_font_size = r.bus_label_font_size; 
    bus_label_offset = r.bus_label_offset; 
    stop_label_font_size = r.stop_label_font_size; 
    stop_label_offset = r.stop_label_offset; 
    underlayer_color = r.underlayer_color;
    underlayer_width = r.underlayer_width; 
    color_palette = r.color_palette;

    return *this;
}

void MapRenderer::SetSettings(const MapSettings& map_settings) {
    map_settings_ = map_settings;
}

void MapRenderer::RoutRender(svg::Document& doc, 
        const SphereProjector& spher_proj, 
        const std::map<std::string, std::vector<const geo::Coordinates*>>& all_buses) const {
    size_t colors_qty = map_settings_.color_palette.size();
    size_t color_index = 0;

    for (const auto& [bus_name, vec_coords] : all_buses) {
        if (vec_coords.empty()) {
        continue;
    }
        svg::Polyline poly;
        for (size_t i = 0; i < vec_coords.size(); ++i) {
            svg::Point point = spher_proj(*vec_coords[i]);
            poly.AddPoint(point);
        }
        poly.SetFillColor(svg::NoneColor).SetStrokeColor(map_settings_.color_palette[color_index]).
            SetStrokeWidth(map_settings_.line_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND).
            SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        doc.Add(poly);
        color_index = color_index + 1 == colors_qty ? 0 :
                            color_index + 1;
    }
}
void MapRenderer::BusNamesRender(svg::Document& doc, 
        const SphereProjector& spher_proj, 
        const std::map<std::string, std::vector<const geo::Coordinates*>>& all_buses, 
        const std::map<std::string, std::vector<size_t>>& bus_end_stops_indexes) const {
    size_t colors_qty = map_settings_.color_palette.size();
    size_t color_index = 0;
    for (const auto& [bus_name, vec_stops] : bus_end_stops_indexes) {
        if (vec_stops.empty()) {continue;}
        for (const size_t& stop_ind : vec_stops) {
            svg::Point stop_p = spher_proj(*all_buses.at(std::string(bus_name))[stop_ind]);

            const svg::Text text = svg::Text().SetFontFamily("Verdana")
                .SetFontSize(map_settings_.bus_label_font_size)
                .SetFontWeight("bold")
                .SetPosition({stop_p.x, stop_p.y})
                .SetData(std::string(bus_name))
                .SetOffset(map_settings_.bus_label_offset);
                doc.Add(svg::Text{text}
                        .SetStrokeColor(map_settings_.underlayer_color)
                        .SetFillColor(map_settings_.underlayer_color)
                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                        .SetStrokeWidth(map_settings_.underlayer_width));
                doc.Add(svg::Text{text}
                        .SetFillColor(map_settings_.color_palette[color_index]));
            }
            color_index = color_index + 1 == colors_qty ? 0 :
                            color_index + 1;
        }
}
void MapRenderer::StopsRender(svg::Document& doc, const SphereProjector& spher_proj,
        const std::map<std::string, const geo::Coordinates*>& stops_coords) const {
    for(const auto& [stop_name, coords] : stops_coords) {
        svg::Point circle_center = spher_proj(*coords);
        svg::Circle circle = svg::Circle().SetCenter(circle_center)
                                            .SetRadius(map_settings_.stop_radius);
        doc.Add(svg::Circle{circle}
                            .SetFillColor(svg::Color{"white"}));
    }
}
void MapRenderer::StopNamesRender(svg::Document& doc, const SphereProjector& spher_proj,
    const std::map<std::string, const geo::Coordinates*>& stops_coords) const {
    for (const auto& [stop_name, coord_ptr] : stops_coords) {
        svg::Point stop_p = spher_proj(*coord_ptr);

        const svg::Text text = svg::Text().SetFontFamily("Verdana")
            .SetFontSize(map_settings_.stop_label_font_size)
            .SetPosition({stop_p.x, stop_p.y})
            .SetData(stop_name)
            .SetOffset(map_settings_.stop_label_offset);
        doc.Add(svg::Text{text}
            .SetStrokeColor(map_settings_.underlayer_color)
            .SetFillColor(map_settings_.underlayer_color)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeWidth(map_settings_.underlayer_width));
        doc.Add(svg::Text{text}
            .SetFillColor(svg::Color{"black"}));
    }
}

const MapSettings* MapRenderer::GetSettings() const {
    return &map_settings_;
}