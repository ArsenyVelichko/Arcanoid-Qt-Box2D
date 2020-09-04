#pragma once

#include "GameObject.h"

class Ball;
class Paddle;
class Bonus;
struct BonusData;

class BonusManager {
public:
  BonusManager(WorldScene* worldScene);
  ~BonusManager();

  void update();
  WorldScene* getWorldScene() const;
  void addBonus(Bonus* bonus);

private:
  QList<Bonus*> m_bonusList;
  WorldScene* m_worldScene;
};

class Bonus {
public:
  Bonus(BonusManager* manager);

  virtual void activate() = 0;
  virtual void update() {}
  virtual void deactivate() {}
  virtual ~Bonus() {}

protected:
  friend class BonusManager;

  BonusManager* m_manager;
  bool m_isDeleted;
  bool m_isActivated;
};

class DoubleBall : public Bonus {
public:
  DoubleBall(QList<Ball*>&& m_ballList, BonusManager* manager);

  void activate() override;

private:
  QList<Ball*>& m_ballList;
};

class AccelerateBall : public Bonus {
public:
  AccelerateBall(float acceleration, QList<Ball*>&& ballList, BonusManager* manager);

  void activate() override;

private:
  QList<Ball*>& m_ballList;
  float m_acceleration;
};

class AddBottom : public Bonus {
public:
  AddBottom(const QPointF& pos, const QSizeF& rect, BonusManager* manager);

  void activate() override;

private:
  QSizeF m_sizes;
  QPointF m_pos;
};

class TmpBonus : public Bonus {

public:
  TmpBonus(float duration, BonusManager* manager);

  void activate() override;
  void update() override;

private:
  float m_remainingTime;
  float m_timeStep;
};

class RandomDirect : public TmpBonus {
public:
  RandomDirect(float probability, QList<Ball*>&& ballList, float duration, BonusManager* manager);

  void update() override;

private:
  QList<Ball*>& m_ballList;
  float m_probability;

  void changeDirect() const;
};

class StickyPaddle : public TmpBonus {
public:
  StickyPaddle(Paddle* paddle, float duration, BonusManager* manager);

  void activate() override;
  void deactivate() override;

private:
  Paddle* m_paddle;
};

class ExpandPaddle : public Bonus {
public:
  ExpandPaddle(Paddle* paddle, float expansion, BonusManager* manager);

  void activate() override;

private:
  Paddle* m_paddle;
  float m_expansion;
};

class FallingBonus : public GameObject {
public:
  FallingBonus(const BonusData& bonusData, WorldScene* worldScene);

  void advance(int phase) override;
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  void beginContact(GameObject* other, b2Contact* contact) override;
  QString getTag() const override;

private:
  QRectF m_rect;
  Bonus* m_bonus;
  bool m_isCatched;
};


inline WorldScene* BonusManager::getWorldScene() const {
  return m_worldScene;
}

inline void BonusManager::addBonus(Bonus* bonus) {
  m_bonusList.append(bonus);
}

inline QRectF FallingBonus::boundingRect() const {
  return m_rect;
}

