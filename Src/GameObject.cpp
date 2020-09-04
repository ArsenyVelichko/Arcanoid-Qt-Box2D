#include <QtMath>
#include <box2d/b2_fixture.h>
#include "GameObject.h"
#include "WorldScene.h"

using namespace SuppModule;

constexpr float g_speedEpsilon = 0.001;

GameObject::GameObject(WorldScene* worldScene) {
  m_constSpeedFlag = false;
  m_isDeleted = false;

  m_worldScene = worldScene;
  worldScene->addItem(this);

  m_body = m_worldScene->m_world->CreateBody(&b2BodyDef());
  m_body->SetUserData(this);
}

void GameObject::advance(int phase) {
  if (phase && m_body->IsEnabled()) {
    QPointF pos = fromBox2D(m_body->GetPosition());
    
    QGraphicsItem* parent = parentItem();
    if (parent) {
      pos = parent->mapFromScene(pos);
    }

    QGraphicsItem::setPos(pos);
    float angle = qRadiansToDegrees(m_body->GetAngle());
    QGraphicsItem::setRotation(angle);
  }
}

void GameObject::setPos(const QPointF& pos) {
  QGraphicsItem::setPos(pos);

  QGraphicsItem* parent = parentItem();
  QPointF scenePos = pos;
  if (parent) {
    scenePos = parent->mapToScene(pos);
  }

  m_body->SetTransform(toBox2D(scenePos), m_body->GetAngle());
}

void GameObject::setRotation(float angle) {
  QGraphicsItem::setRotation(angle);
  float radians = qDegreesToRadians(angle);
  m_body->SetTransform(m_body->GetPosition(), radians);
}

void GameObject::setConstSpeed(bool flag) {
  m_constSpeedFlag = flag;
  if (flag) {
    m_constSpeed = m_body->GetLinearVelocity().Length();
  }
}

void GameObject::setSpeedToConst() const {
  b2Vec2 currVelocity = m_body->GetLinearVelocity();
  float currSpeed = currVelocity.Normalize();
  float diffSpeed = m_constSpeed - currSpeed;

  if (abs(diffSpeed) > g_speedEpsilon) {
    m_body->SetLinearVelocity(m_constSpeed * currVelocity);
  }
}

void GameObject::endContact(GameObject* other, b2Contact* contact) {
  if (m_constSpeedFlag) {
    setSpeedToConst();
  }
}

void GameObject::scheduleDeletion() {
  if (!m_isDeleted) {
    m_isDeleted = true;
    m_worldScene->scheduleDeletion(this);
  }
}