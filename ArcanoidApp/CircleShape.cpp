#include "CircleShape.h"

QRectF CircleShape::aabb() const {
  QRectF rect(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
  return getTransform().mapRect(rect);
}