#pragma once

#include <QAbstractGraphicsShapeItem>
#include <Box2D/b2_body.h>
#include "SuppModule.h"

class WorldScene;

class GameObject : public QAbstractGraphicsShapeItem {
public:
  GameObject(WorldScene* worldScene);

  virtual void beginContact(GameObject* other, b2Contact* contact) {}
  virtual void endContact(GameObject* other, b2Contact* contact) {}
  virtual void preSolve(GameObject* other, b2Contact* contact) {}

  virtual QString getTag() const;

  void advance(int phase) override;
  void setConstSpeed(bool flag);
  void setSpeedToConst() const;

  b2Body* getBody() const;
  WorldScene* getWorldScene() const;
  void scheduleDeletion();

  void setPos(float x, float y);
  void setPos(const QPointF& pos);
  void setRotation(float angle);

private:
  b2Body* m_body;
  WorldScene* m_worldScene;

  bool m_isDeleted;
  bool m_constSpeedFlag;
  float m_constSpeed;
};

inline b2Body* GameObject::getBody() const {
  return m_body;
}

inline WorldScene* GameObject::getWorldScene() const {
  return m_worldScene;
}

inline QString GameObject::getTag() const {
  return "";
}

inline void GameObject::setPos(float x, float y) {
  setPos(QPointF(x, y));
}