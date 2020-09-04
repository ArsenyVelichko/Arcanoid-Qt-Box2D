#include <QPainter>
#include <box2d/b2_edge_shape.h>
#include <box2d/b2_world.h>
#include <box2d/b2_fixture.h>
#include "Surface.h"

using namespace SuppModule;

Surface::Surface(const QList<QPointF>& vertices, WorldScene* worldScene)
  : GameObject(worldScene) {
  m_vertices = vertices;
  b2Body* body = getBody();
  
  b2FixtureDef fixtureDef;
  fixtureDef.density = 1.0f;
  fixtureDef.restitution = 0.0f;
  fixtureDef.friction = 0.0f;

  QPointF leftTop, rightBottom;
  for (int i = 0; i < vertices.size() - 1; i++) {
    QPointF v1 = vertices[i];
    QPointF v2 = vertices[i + 1];

    leftTop = minPoint(leftTop, minPoint(v1, v2));
    rightBottom = maxPoint(rightBottom, maxPoint(v1, v2));

    b2EdgeShape edgeShape;
    edgeShape.SetTwoSided(toBox2D(v1), toBox2D(v2));

    fixtureDef.shape = &edgeShape;
    body->CreateFixture(&fixtureDef);
  }
  m_rect = QRectF(leftTop, rightBottom);
}

void Surface::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  for (int i = 0; i < m_vertices.size() - 1; i++) {
    painter->drawLine(m_vertices[i], m_vertices[i + 1]);
  }
}