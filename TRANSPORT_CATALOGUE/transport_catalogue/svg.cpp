// Вставьте сюда решение из предыдущего спринта
#include "svg.h"
#include <cmath>

namespace svg {

using namespace std::literals;

//-------------Rgb-------------------
std::ostream& operator<<(std::ostream& out, Rgb rgb){
    // rgb(100,200,255)
    out << "rgb("sv << static_cast<int>(rgb.red) << ","sv << static_cast<int>(rgb.green) << ","sv << static_cast<int>(rgb.blue) << ")"sv;
    return out;
}
//-------------Rgba------------------
std::ostream& operator<<(std::ostream& out, Rgba rgba){
    // rgba(100,200,255,0.5)
    out << "rgba("sv << static_cast<int>(rgba.red) << ","sv << static_cast<int>(rgba.green) << ","sv << static_cast<int>(rgba.blue) <<
    ","sv << rgba.opacity << ")"sv;
    return out;
}
//-------------Color-----------------
void OstreamPrinter::operator()(std::monostate) const {
    out << "none"sv;
}
void OstreamPrinter::operator()(std::string color) const {
    out << color;
}
void OstreamPrinter::operator()(Rgb rgb) const {
    out << rgb;
}
void OstreamPrinter::operator()(Rgba rgba) const {
    out << rgba;
}

//-------------StrokeLineCap---------
std::ostream& operator<<(std::ostream& out, const StrokeLineCap& line_cap) {
    switch (line_cap)
    {
    case StrokeLineCap::BUTT: out << "butt"sv; break;
    case StrokeLineCap::ROUND: out << "round"sv; break;
    case StrokeLineCap::SQUARE: out << "square"sv; break;
    default:
        break;
    }
    return out;
}
//--------------StrokeLineJoin----------
std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& line_join) {
    switch (line_join)
    {
    case StrokeLineJoin::ARCS: out << "arcs"sv; break;
    case StrokeLineJoin::BEVEL: out << "bevel"sv; break;
    case StrokeLineJoin::MITER: out << "miter"sv; break;
    case StrokeLineJoin::MITER_CLIP: out << "miter-clip"sv; break;
    case StrokeLineJoin::ROUND: out << "round"sv; break;
    default:
        break;
    }
    return out;
}

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    // Выводим атрибуты, унаследованные от PathProps
    RenderAttrs(context.out);
    out << "/>"sv;
}
//---------Polyline------------
Polyline& Polyline::AddPoint(Point point) {
    points_.emplace_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    bool is_first = true;
    for (const Point& point : points_) {
        if (is_first) {
            out << point.x << ","sv << point.y;
            is_first = false;
            continue;
        }
        out << " "sv << point.x << ","sv << point.y;
    }

    out << "\""sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}
//---------Text----------------
Text& Text::SetPosition(Point pos) {
    set_point_ = pos;
    return *this;
}
Text& Text::SetOffset(Point offset) {
    set_off_point_ = offset;
    return *this;
}
Text& Text::SetFontSize(uint32_t size) {
    font_size_ = size;
    return *this;
}
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}
Text& Text::SetData(std::string data) {
    text_ = data;
    return *this;
}

void Text::RenderObject(const RenderContext& context) const {
    
    auto& out = context.out;
    out << "<text"sv;
    RenderAttrs(context.out);
    out << " x=\""sv << set_point_.x << "\" y=\""sv << set_point_.y;
    out << "\" dx=\""sv << set_off_point_.x << "\" dy=\""sv << set_off_point_.y;
    out << "\" font-size=\""sv << font_size_ << "\""sv;
    if (!font_family_.empty()) {
    out << " font-family=\""sv << font_family_ << "\""sv;}
    if (!font_weight_.empty()) {
    out << " font-weight=\""sv << font_weight_ << "\""sv;}
    out << ">"sv << TextEditor(text_) << "</text>"sv;
}

    std::string Text::TextEditor(const std::string& str) const {
    std::string result = ""s;
    for (const char& c : str) {
        switch (c)
        {
        case '<': result += "&lt;"s; break;
        case '>': result += "&qt;"s; break;
        case '"': result += "&quot;"s; break;
        case '\'': result += "&apos;"s; break;
        case '&': result += "&amp;"s; break;
        default: result += c; break;
        }
        
    }
    return result;
}
//---------Document------------
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
   objects_.emplace_back(std::move(obj)); 
}

void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
    for (const auto& object : objects_) {
        object->Render(out);
    }
    out << "</svg>"sv;
}

}  // namespace svg

