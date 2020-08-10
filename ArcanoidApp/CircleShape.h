#pragma once

#include "Shape.h"

class CircleShape : public Shape {
public:
  CircleShape(qreal radius) : Shape(Shape::Circle), m_radius(radius) {}

  virtual QRectF aabb() const;

private:
  qreal m_radius;
};