#include "svg.h"

namespace Svg {

  Color::Color() : _color("none")
  {}

  Color::Color(const std::string& color) : _color(color)
  {}
  
  Color::Color(const char* color) : _color(color)
  {}

  Color::Color(const Rgb& rgb) : _color(rgb) 
  {}

  std::ostream& operator<<(std::ostream& os, const Rgb& color) {
    os<<"rgb("
      <<std::to_string(color.red)<<","
      <<std::to_string(color.green)<<","
      <<std::to_string(color.blue)
      <<")";
    return os;
  }

  std::ostream& operator<<(std::ostream& os, const Color& color) {
    switch (color._color.index()){
      case 0:         //as string    
        os<<std::get<std::string>(color._color);
        break;
      case 1:         //as Rgb 
        os<<std::get<Rgb>(color._color);
        break;
      default:
        break;
    }  
    return os;
  }



  Object& Object::SetFillColor(const Color& fill_color) {
    _fill_color = fill_color;
    return *this;
  }

  Object& Object::SetStrokeColor(const Color& stroke_color) {
    _stroke_color = stroke_color;
    return *this;
  }

  Object& Object::SetStrokeWidth(double stroke_width) {
    _stroke_width = stroke_width;
    return *this;
  }

  Object& Object::SetStrokeLineCap(const std::string& line_cap) {
    _line_cap = line_cap;
    return *this;
  }

  Object& Object::SetStrokeLineJoin(const std::string& line_join) {
    _line_join = line_join;
    return *this;
  }

  std::ostream& operator<<(std::ostream& os, const Object& obj) {
    if (typeid(obj).name() == typeid(Circle).name()){      
        os<<"<circle";
    }
    else if (typeid(obj).name() == typeid(Polyline).name()){    
        os<<"<polyline";
    }
    else if (typeid(obj).name() == typeid(Text).name()){      
        os<<"<text";
    }
    os<<" fill=\""<<obj._fill_color<<"\" stroke=\""<<obj._stroke_color<<"\" stroke-width=\""<<obj._stroke_width<<"\"";
    if (obj._line_cap.has_value()){
      os<<" stroke-linecap=\""<<obj._line_cap.value()<<"\"";
    }
    if (obj._line_join.has_value()){
      os<<" stroke-linejoin=\""<<obj._line_join.value()<<"\"";
    }

    obj.render_object(os);
    if (!(typeid(obj).name() == typeid(Text).name())){      
      os<<" />";
    }
    return os;
  }

  std::ostream& Object::render_object(std::ostream& os) const {
    os<<(*this);
    return os;
  }


  Circle& Circle::SetCenter(const Point& center) {
    _center = center;
    return *this;
  }

  Circle& Circle::SetRadius(double radius) {
    _radius = radius;
    return *this;
  }


  std::ostream& Circle::render_object(std::ostream& os) const  {
    os<<" cx=\""<<_center.x<<"\" cy=\""<<_center.y<<"\" r=\""<<_radius<<"\"";
    return os;
  }

  Polyline& Polyline::AddPoint(const Point& point){
    _points.push_back(point);
    return *this;
  }

  std::ostream& Polyline::render_object(std::ostream& os) const  {
    os<<" points=\"";
    for (auto& point : _points){
      os<<point.x<<","<<point.y<<" ";
    }
    os<<"\"";
    return os;
  }

  Text& Text::SetPoint(const Point& point) {
    _point = point;
    return *this;
  }

  Text& Text::SetOffset(const Point& offset) {
    _offset = offset;
    return *this;
  }

  Text& Text::SetFontSize(uint32_t font_size) {
    _font_size = font_size;
    return *this;
  }

  Text& Text::SetFontFamily(const std::string& font_family) {
    _font_family = font_family;
    return *this;
  }

  Text& Text::SetData(const std::string& data) {
    _data = data;
    return *this;
  }

  std::ostream& Text::render_object(std::ostream& os) const  {
    os<<" x=\""<<_point.x<<"\" y=\""<<_point.y<<"\" dx=\""<<_offset.x<<"\" dy=\""<<_offset.y<<
      "\" font-size=\""<<_font_size<<"\"";
    if (_font_family.has_value()){
      os<<" font-family=\""<<_font_family.value()<<"\"";
    }

    os<<" >"<<_data<<"</text>";
    return os;
  }


  void Document::Add(Object&& object) {
    _objects.push_back(object.Move());
  }

  void Document::Add(const Object& object) {
    _objects.push_back(object.Clone());
  }

  void Document::Render(std::ostream& os) const {
    os<<"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
    os<<"<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">";
    for(const auto& obj : _objects){
      os<<*obj;
    }
    os<<"</svg>";
  }

  void Document::Clear() {
    _objects.clear();
  }

}   //namespace Svg