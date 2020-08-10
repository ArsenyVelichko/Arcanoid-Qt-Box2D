#pragma once

#include <QTransform>

class Shape {
public:
  enum Type {
    Circle,
    Rect,
  };

  Shape(Shape::Type type) : m_type(type) {}
  virtual ~Shape() {}

  Shape::Type getType() const { return m_type; }
  QTransform getTransform() const { return m_tranform; }
  virtual QRectF aabb() const = 0;

  void setTransform(const QTransform& tranform) { m_tranform = tranform; }

private:
  Type m_type;
  QTransform m_tranform;
};