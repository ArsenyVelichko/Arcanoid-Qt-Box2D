#pragma once

#include "GameObject.h"

class Ball : public GameObject {
public:
  Ball(float radius, const QPointF& startV, WorldScene* worldScene);

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  QString getTag() const override;

  float getRadius() const;
  void accelerate(float acceleration);

private:
  float m_radius;
};

inline QRectF Ball::boundingRect() const {
  return QRectF(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
}

inline float Ball::getRadius() const {
  return m_radius;
}

inline QString Ball::getTag() const {
  return "Ball";
}