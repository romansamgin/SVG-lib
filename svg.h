#include <cstdint>
#include <string>
#include <variant>
#include <memory>
#include <optional>
#include <vector>
#include <iostream>

namespace Svg {

struct Point {
  double x;
  double y;
};      

struct Rgb {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

class Color {
  public:
    Color();
    Color(const std::string&);
    Color(const char*);
    Color(const Rgb&);
  private:
    using ColorType = std::variant<std::string, Rgb>;
    ColorType _color = "none";
    friend std::ostream& operator<<(std::ostream& os, const Color& color);
};
  
static const Color NoneColor;


class Object {
  public:
    Object() = default;
    Object(const Object&) = default;
    Object(Object&&) = default;
    Object& operator=(const Object&) = default;
    Object& operator=(Object&&) = default;

    Object& SetFillColor(const Color&);
    Object& SetStrokeColor(const Color&);
    Object& SetStrokeWidth(double);
    Object& SetStrokeLineCap(const std::string&);
    Object& SetStrokeLineJoin(const std::string&);

    virtual std::unique_ptr<Object> Clone() const = 0;
    virtual std::unique_ptr<Object> Move() = 0;
    virtual ~Object() {};

  protected:
    virtual std::ostream& render_object(std::ostream& os) const;

  private:
    Color _fill_color = NoneColor;
    Color _stroke_color = NoneColor;
    double _stroke_width = 1.0;
    std::optional<std::string> _line_cap;
    std::optional<std::string> _line_join;
    friend std::ostream& operator<<(std::ostream& os, const Object& obj);
};

template<class Derived>
class ObjectProxy : public Object{
  public:
    Derived& SetFillColor(const Color& color) {
      return static_cast<Derived&>(Object::SetFillColor(color));
    }

    Derived& SetStrokeColor(const Color& color) {
      return static_cast<Derived&>(Object::SetStrokeColor(color));
    }

    Derived& SetStrokeWidth(double width) {
      return static_cast<Derived&>(Object::SetStrokeWidth(width));
    }

    Derived& SetStrokeLineCap(const std::string& line_cap) {
      return static_cast<Derived&>(Object::SetStrokeLineCap(line_cap));
    }

    Derived& SetStrokeLineJoin(const std::string& line_join) {
      return static_cast<Derived&>(Object::SetStrokeLineJoin(line_join));
    }

    std::unique_ptr<Object> Clone() const override {
      return std::make_unique<Derived>(static_cast<const Derived&>(*this));
    };

    std::unique_ptr<Object> Move() override {
      return std::make_unique<Derived>(std::move(static_cast<Derived&>(*this)));
    };
};

class Circle : public ObjectProxy<Circle> {
  public:
    Circle& SetCenter(const Point&);
    Circle& SetRadius(double);
  private:
    Point _center{0.0, 0.0};
    double _radius{1.0};
    virtual std::ostream& render_object(std::ostream& os) const override;
};

class Polyline : public ObjectProxy<Polyline> {
  public:
    Polyline& AddPoint(const Point&);

  private:
    std::vector<Point> _points;
    virtual std::ostream& render_object(std::ostream& os) const override;
};

class Text : public ObjectProxy<Text> {
  public:
    Text& SetPoint(const Point&);
    Text& SetOffset(const Point&);
    Text& SetFontSize(uint32_t);
    Text& SetFontFamily(const std::string&);
    Text& SetData(const std::string&);

  private:
    Point _point{0.0, 0.0};
    Point _offset{0.0, 0.0};
    uint32_t _font_size{1};
    std::optional<std::string> _font_family;
    std::string _data = "";
    virtual std::ostream& render_object(std::ostream& os) const override;
};

class Document {
  public:
    void Add(Object&& object);
    void Add(const Object& object);
    void Render(std::ostream& os) const;
    void Clear();
  private:
    std::vector<std::unique_ptr<Object>> _objects;
};





}   //namespace Svg