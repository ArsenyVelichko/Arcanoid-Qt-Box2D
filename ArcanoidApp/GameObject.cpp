#include "GameObject.h"

GameObject::GameObject(const GameObjDef& definition, QGraphicsItem* parent)
  : QAbstractGraphicsShapeItem(parent) {
  m_direction = definition.direction;
  m_velocity = definition.velocity;
  m_isHittable = definition.isHittable;
  m_type = definition.type;
}

QPainterPath GameObject::shape() const {
  QPainterPath path;

  for (auto shape : m_shapeList) {
    path.addRect(shape->aabb());
  }
  return path;
}

bool isCollide(const GameObject* other, Manifold& manifold) {

}