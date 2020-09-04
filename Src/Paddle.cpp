#include <QPainter>
#include <QtMath>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_prismatic_joint.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_contact.h>
#include "Paddle.h"
#include "GameCallbacks.h"
#include "WorldScene.h"
#include "Ball.h"

using namespace SuppModule;

constexpr float g_maxAngle = 3.0f / 8.0f * b2_pi;
constexpr float g_minAngle = 1.0f / 8.0f * b2_pi;
constexpr uint16 g_paddleBitCat = 0x002;
constexpr float g_paddleDensity = 100.0f;
constexpr float g_repulsionCoef = 0.01;

constexpr float g_shadowInterval = 0.5f;

struct StickyInfo {
  Ball* ball;
  b2Fixture* attachedFixture;
  QGraphicsItem* prevParent;
};

struct CollisionInfo {
  b2Vec2 localPos;
  Ball* ball;
};

Paddle::Paddle(const PaddleDef& def, WorldScene* worldScene)
  : GameObject(worldScene) {
  b2Vec2 axis = def.axis;
  axis.Normalize();
  m_velocity = toBox2D(def.speed) * axis;
  m_isSticky = false;
  m_shadow = nullptr;
  m_direction = 0;

  setPos(def.origin);
  setRotation(qRadiansToDegrees(acos(axis.x)));

  b2Body* body = getBody();
  body->SetType(b2_dynamicBody);
  body->SetFixedRotation(true);

  b2PolygonShape box;
  m_rect = centredAabb(def.sizes);
  box.SetAsBox(toBox2D(m_rect.right()), toBox2D(m_rect.bottom()));

  b2FixtureDef fd;
  fd.shape = &box;
  fd.density = g_paddleDensity;
  fd.restitution = 0.0f;
  fd.friction = 0.0f;
  fd.filter.categoryBits = g_paddleBitCat;
  m_mainFixture = body->CreateFixture(&fd);

  b2PrismaticJointDef jointDef;
  jointDef.collideConnected = true;
  jointDef.Initialize(body, def.pillarBody, toBox2D(def.origin), axis);
  body->GetWorld()->CreateJoint(&jointDef);
}

void Paddle::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  painter->setBrush(Qt::gray);
  painter->drawRect(m_rect);
}

void Paddle::advance(int phase) {
  if (phase) {

    if (m_shadow && m_shadow->isObscured()) {
      delete m_shadow;
      m_shadow = nullptr;
    }

    processCollidedBalls();
    getBody()->SetLinearVelocity(m_direction * m_velocity);
    setConstSpeed(true);
  }
  GameObject::advance(phase);
}

bool Paddle::isDirectHit(b2Contact* contact) {
  if (contact->GetFixtureA() == m_mainFixture ||
      contact->GetFixtureB() == m_mainFixture) {
    return true;
  }
  return false;
}

void Paddle::beginContact(GameObject* other, b2Contact* contact) {
  if (other->getTag() == "Ball") {

    if (!isDirectHit(contact)) { return; }

    Ball* ball = static_cast<Ball*>(other);
    b2Vec2 ballPos = ball->getBody()->GetPosition();
    b2Vec2 localPos = b2MulT(getBody()->GetTransform(), ballPos);
    float scaledLocalY = fromBox2D(localPos.y);
    float halfHeight = m_rect.height() / 2;

    if (scaledLocalY < -halfHeight) {
      CollisionInfo ci;
      ci.ball = ball;
      ci.localPos = localPos;
      m_collidedBalls.append(ci);

    } else if (scaledLocalY < halfHeight && m_direction) {
      m_ignoredContacts.append(contact);
    }
  }
}

void Paddle::preSolve(GameObject* other, b2Contact* contact) {
  if (m_ignoredContacts.contains(contact)) {
    contact->SetEnabled(false);
  }
}

void Paddle::endContact(GameObject* other, b2Contact* contact) {
  m_ignoredContacts.removeOne(contact);
  GameObject::endContact(other, contact);
}

StickyInfo Paddle::stickBall(const CollisionInfo& ci) {
  Ball* ball = ci.ball;

  QGraphicsItem* prevParent = ball->parentItem();
  ball->setParentItem(this);
  ball->setPos(fromBox2D(ci.localPos));
  
  b2Body* ballBody = ball->getBody();
  b2Body* selfBody = getBody();

  b2CircleShape circle;
  circle.m_p = ci.localPos;
  circle.m_radius = toBox2D(ball->getRadius());
  
  b2FixtureDef fd;
  fd.shape = &circle;
  fd.density = g_paddleDensity;
  fd.friction = 0.0f;
  fd.restitution = 0.0f;
  fd.filter.categoryBits = g_paddleBitCat;
  ballBody->SetEnabled(false);

  StickyInfo si;
  si.attachedFixture = selfBody->CreateFixture(&fd);
  si.ball = ball;
  si.prevParent = prevParent;
  return si;
}

void Paddle::reflectBall(const CollisionInfo& ci) const {
  Ball* ball = ci.ball;
  float maxX = toBox2D(m_rect.width() / 2 + ball->getRadius());
  float ratio = -ci.localPos.x / maxX;

  int k = ratio > 0 ? 1 : -1;
  float bounceAngle = k * g_minAngle + ratio * (g_maxAngle - g_minAngle);
  bounceAngle += b2_pi / 2 - getBody()->GetAngle();

  b2Vec2 newDirect = b2Vec2(cos(bounceAngle), -sin(bounceAngle));
  ball->getBody()->SetLinearVelocity(newDirect);
}

void Paddle::processCollidedBalls() {
  for (auto ci : m_collidedBalls) {
    reflectBall(ci);

    float halfWidth = toBox2D(m_rect.width() / 2);
    if (m_isSticky && abs(ci.localPos.x) < halfWidth) {
      m_stickyInfoList.append(stickBall(ci));
    }
  }
  m_collidedBalls.clear();
}

void Paddle::launchBall() {
  if (m_stickyInfoList.isEmpty()) { return; }

  StickyInfo si = m_stickyInfoList.takeLast();
  b2CircleShape* circle = static_cast<b2CircleShape*>(si.attachedFixture->GetShape());
  QPointF pos = fromBox2D(circle->m_p);
  
  Ball* ball = si.ball;
  b2Body* ballbody = ball->getBody();
  QPointF newPos = mapToItem(si.prevParent, ball->pos());
  newPos += fromBox2D(ballbody->GetLinearVelocity()) * g_repulsionCoef;
  ball->setParentItem(si.prevParent);
  ball->setPos(newPos);

  getBody()->DestroyFixture(si.attachedFixture);
  ballbody->SetEnabled(true);
}

void Paddle::setSticky(bool flag) {
  m_isSticky = flag;

  if (!flag) {
    while (!m_stickyInfoList.isEmpty()) {
      launchBall();
    }
  }
}

void Paddle::expand(float expansion) {
  if (m_shadow) {
    m_shadow->expand(expansion);

  } else {
    m_shadow = new PaddleShadow(this, expansion);
  }
}

PaddleShadow::PaddleShadow(Paddle* paddle, float expansion) {
  m_paddle = paddle;
  m_isObscured = false;

  QRectF rect = paddle->m_rect.adjusted(-expansion, 0, expansion, 0);
  setRect(rect);
  setPen(Qt::DashLine);
  setParentItem(paddle);

  m_remainingTime = g_shadowInterval;
  m_timeStep = paddle->getWorldScene()->getTimeStep();

  b2PolygonShape box;
  float hWidth = rect.width() / 2;
  float hHeight = rect.height() / 2;
  box.SetAsBox(toBox2D(hWidth), toBox2D(hHeight));

  b2FixtureDef fd;
  fd.shape = &box;
  fd.density = g_paddleDensity;
  fd.filter.categoryBits = g_paddleBitCat;
  fd.isSensor = true;

  b2Body* paddleBody = paddle->getBody();
  m_expandedFixture = paddleBody->CreateFixture(&fd);

  checkSpace();
}

void PaddleShadow::advance(int phase) {
  if (phase) {
    checkSpace();

    m_remainingTime -= m_timeStep;
    if (m_remainingTime <= 0) {
      setVisible(!isVisible());
      m_remainingTime += g_shadowInterval;
    }
  }
}

void PaddleShadow::expand(float expansion) {
  m_isObscured = false;

  QRectF newRect = rect().adjusted(-expansion, 0, expansion, 0);
  setRect(newRect);

  b2Shape* shape = m_expandedFixture->GetShape();
  b2PolygonShape* box = static_cast<b2PolygonShape*>(shape);
  float hWidth = newRect.width() / 2;
  float hHeight = newRect.height() / 2;
  box->SetAsBox(toBox2D(hWidth), toBox2D(hHeight));

  checkSpace();
}

void PaddleShadow::checkSpace() {
  b2Filter filter;
  filter.maskBits &= ~g_paddleBitCat;

  IsOccupiedQuery query(filter);
  b2World* world = m_expandedFixture->GetBody()->GetWorld();
  QRectF sceneRect = mapRectToScene(rect());
  world->QueryAABB(&query, toBox2D(sceneRect));

  if (!query.isOccupied()) {
    passToPaddle();
  }
}

void PaddleShadow::passToPaddle() {
  b2Body* paddleBody = m_paddle->getBody();
  paddleBody->DestroyFixture(m_paddle->m_mainFixture);

  m_expandedFixture->SetSensor(false);
  m_paddle->m_mainFixture = m_expandedFixture;
  m_paddle->m_rect = rect();

  m_isObscured = true;
}