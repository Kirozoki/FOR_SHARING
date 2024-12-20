// Вставьте сюда решение из предыдущего спринта
#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <deque>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace svg {
struct Rgb {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};
std::ostream& operator<<(std::ostream& out, Rgb rgb);

struct Rgba{
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
};
std::ostream& operator<<(std::ostream& out, Rgba rgba);

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
inline const Color NoneColor{"none"};

struct OstreamPrinter {
    std::ostream& out;

    void operator()(std::monostate) const;
    void operator()(std::string color) const;
    void operator()(Rgb rgb) const;
    void operator()(Rgba rgba) const;
};

struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};
std::ostream& operator<<(std::ostream& out, const StrokeLineCap& line_cap);

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& line_join);

template <typename Owner>
class PathProps {
    public:
    Owner& SetFillColor(Color fill_color) {
        if (fill_color.index() != 0) {
            fill_color_ = fill_color;}
        return AsOwner();
    }

    Owner& SetStrokeColor(Color stroke_color) {
        if (stroke_color.index() != 0) {
            stroke_color_ = stroke_color;}
        return AsOwner();
    }

    Owner& SetStrokeWidth(double width) {
        stroke_width_ = width;
        return AsOwner();
    }

    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        stroke_l_cap_ = line_cap;
        return AsOwner();
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        stroke_l_join_ = line_join;
        return AsOwner();
    }

    protected:
    ~PathProps() = default;

    // Метод RenderAttrs выводит в поток общие для всех путей атрибуты fill и stroke
    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;
        if (fill_color_) {
            out << " fill=\""sv;
            std::visit(OstreamPrinter{out},fill_color_.value()); 
            out << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv;
            std::visit(OstreamPrinter{out}, stroke_color_.value()); 
            out << "\""sv;
        }

        if (stroke_width_) {
            out << " stroke-width=\""sv << stroke_width_.value() << "\""sv;
        }

        if (stroke_l_cap_) {
            out << " stroke-linecap=\""sv << stroke_l_cap_.value() << "\""sv;
        }

        if (stroke_l_join_) {
            out << " stroke-linejoin=\""sv << stroke_l_join_.value() << "\""sv;
        }
    }

    private:
    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> stroke_l_cap_;
    std::optional<StrokeLineJoin> stroke_l_join_;

};

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};


/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline> {
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);

    /*
     * Прочие методы и данные, необходимые для реализации элемента <polyline>
     */
private:
void RenderObject(const RenderContext& context) const override;

std::deque<Point> points_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final: public Object, public PathProps<Text> {
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

    // Прочие данные и методы, необходимые для реализации элемента <text>
private:
    void RenderObject(const RenderContext& context) const override;
    std::string TextEditor(const std::string& str) const;
    Point set_point_ = {0, 0};
    Point set_off_point_ = {0, 0};
    uint32_t font_size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string text_;
};

class ObjectContainer {
    public:

    virtual ~ObjectContainer() = default;

    template <typename Obj>
    void Add(Obj obj) {
        AddPtr(std::make_unique<Obj>(std::move(obj)));                
    } 

    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    
};

class Document : public ObjectContainer {
public:

    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;
    private:
    std::vector<std::unique_ptr<Object>> objects_;

};

class Drawable {
    public:
    virtual ~Drawable() = default;
    virtual void Draw(ObjectContainer& obj_cont) const = 0;

};

}  // namespace svg

