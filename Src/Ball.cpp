#include <QPainter>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_world.h>
#include <box2d/b2_fixture.h>
#include "Ball.h"

using namespace SuppModule;

constexpr float g_ballDensity = 1.0f;

Ball::Ball(float radius, const QPointF& startV, WorldScene* worldScene)
  : GameObject(worldScene) {
  setZValue(1);

  b2Body* body = getBody();
  body->SetType(b2_dynamicBody);
  body->SetLinearVelocity(toBox2D(startV));
  body->SetBullet(true);
  setConstSpeed(true);

  b2CircleShape circle;
  m_radius = radius;
  circle.m_radius = toBox2D(radius);

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &circle;

  fixtureDef.density = g_ballDensity;
  fixtureDef.restitution = 1.0f;
  fixtureDef.friction = 0.0f;

  body->CreateFixture(&fixtureDef);
}

void Ball::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  painter->setBrush(QColor(220, 20, 60));
  QRectF rect(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
  painter->drawEllipse(rect);
}

void Ball::accelerate(float acceleration) {
  acceleration = toBox2D(acceleration);

  b2Body* body = getBody();
  b2Vec2 currV = body->GetLinearVelocity();
  float newSpeed = currV.Normalize() + acceleration;
  body->SetLinearVelocity(newSpeed * currV);
  setConstSpeed(true);
}