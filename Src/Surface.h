#pragma once

#include "GameObject.h"

class Surface : public GameObject {
public:
  Surface(const QList<QPointF>& vertices, WorldScene* worldScene);

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
  QList<QPointF> m_vertices;
  QRectF m_rect;
};

inline QRectF Surface::boundingRect() const {
  return m_rect;
}