#pragma once

#include <QAbstractGraphicsShapeItem>
#include "Shape.h"
#include <QVector2D>

enum class GameObjType {
  Static,
  Kinematic,
};

struct GameObjDef {
  QVector2D direction;
  qreal velocity = 0.0;
  bool isHittable = true;
  GameObjType type = GameObjType::Static;
};

struct CollisionRecord {
  QVector2D normal;
  QPointF point;
  qreal separation;
};

class GameObject : public QAbstractGraphicsShapeItem {
public:
  GameObject(const GameObjDef& definition, QGraphicsItem* parent = nullptr);

  void addShape(Shape* shape) { m_shapeList.append(shape); }
  bool isHittable() const { return m_isHittable; }

  QPainterPath shape() const override;

  bool isCollide(const GameObject* other, CollisionRecord& record);
  virtual void collisionResponse(const CollisionRecord& record, GameObject* collider) = 0;

private:
  QList<Shape*> m_shapeList;
  QVector2D m_direction;
  qreal m_velocity;
  bool m_isHittable;
  GameObjType m_type;
};