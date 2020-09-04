#pragma once

#include "GameObject.h"

class Ball;
class PaddleShadow;
struct StickyInfo;
struct CollisionInfo;

struct PaddleDef {
  QSizeF sizes;
  QPointF origin;
  b2Vec2 axis;
  b2Body* pillarBody;
  float speed;
};

class Paddle : public GameObject {
public:
  Paddle(const PaddleDef& def, WorldScene* worldScene);

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
  void advance(int phase) override;

  void beginContact(GameObject* other, b2Contact* contact) override;
  void preSolve(GameObject* other, b2Contact* contact) override;
  void endContact(GameObject* other, b2Contact* contact) override;

  QString getTag() const override;

  void switchLeft();
  void switchRight();
  void launchBall();
  void setSticky(bool flag);
  void expand(float expansion);

private:
  friend class PaddleShadow;

  QRectF m_rect;
  b2Vec2 m_velocity;
  int m_direction;

  bool m_isSticky;
  b2Fixture* m_mainFixture;
  QList<CollisionInfo> m_collidedBalls;
  QList<StickyInfo> m_stickyInfoList;
  QList<b2Contact*> m_ignoredContacts;

  PaddleShadow* m_shadow;

  void processCollidedBalls();
  bool isDirectHit(b2Contact* contact);
  StickyInfo stickBall(const CollisionInfo& ci);
  void reflectBall(const CollisionInfo& ci) const;
};

class PaddleShadow : public QGraphicsRectItem {
public:
  PaddleShadow(Paddle* paddle, float expansion);
  
  void advance(int phase) override;
  void expand(float expansion);
  bool isObscured() const;

private:
  b2Fixture* m_expandedFixture;
  Paddle* m_paddle;
  bool m_isObscured;

  void checkSpace();
  void passToPaddle();
    
  float m_remainingTime;
  float m_timeStep;
};

inline QRectF Paddle::boundingRect() const {
  return m_rect;
}

inline void Paddle::switchLeft() {
  m_direction = qMax(-1, m_direction - 1);
}

inline void Paddle::switchRight() {
  m_direction = qMin(1, m_direction + 1);
}

inline QString Paddle::getTag() const {
  return "Paddle";
}

inline bool PaddleShadow::isObscured() const {
  return m_isObscured;
}