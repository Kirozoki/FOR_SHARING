// Вставьте сюда решение из предыдущего спринта
#pragma once
#include "svg.h"
#include "geo.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <optional>
#include <vector>

inline const double EPSILON = 1e-6;
bool IsZero(double value);

class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    SphereProjector() = default;
    
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const ;

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

struct MapSettings
{
    double width = 0.0; // от 0 до 100000
    double height = 0.0; // от 0 до 100000
    double padding = 0.0; // от 0 до min(width, height)/2
    double line_width = 0.0; // от 0 до 100000
    double stop_radius = 0.0; // от 0 до 100000
    int bus_label_font_size = 0; // от 0 до 100000
    svg::Point bus_label_offset = {0.0, 0.0}; // от –100000 до 100000
    int stop_label_font_size = 0; // от 0 до 100000
    svg::Point stop_label_offset = {0.0, 0.0}; // от –100000 до 100000
    svg::Color underlayer_color = svg::NoneColor;
    double underlayer_width = 0.0; // от 0 до 100000
    std::vector<svg::Color> color_palette {};

    MapSettings() = default;

    MapSettings& operator=(const MapSettings& r) ;
};




class MapRenderer {

public:
    MapRenderer() = default;
    
    ~MapRenderer() = default;

    void SetSettings(const MapSettings& map_settings);
    
    void RoutRender(svg::Document& doc, const SphereProjector& spher_proj, 
        const std::map<std::string, std::vector<const geo::Coordinates*>>& all_buses) const;
    void BusNamesRender(svg::Document& doc, const SphereProjector& spher_proj, 
        const std::map<std::string, std::vector<const geo::Coordinates*>>& all_buses, 
        const std::map<std::string, std::vector<size_t>>& bus_end_stops_indexes) const;
    void StopsRender(svg::Document& doc, const SphereProjector& spher_proj, 
        const std::map<std::string, const geo::Coordinates*>& stops_coords) const;
    void StopNamesRender(svg::Document& doc, const SphereProjector& spher_proj, 
        const std::map<std::string, const geo::Coordinates*>& stops_coords) const;

    const MapSettings* GetSettings() const;

private:
    MapSettings map_settings_;
};
