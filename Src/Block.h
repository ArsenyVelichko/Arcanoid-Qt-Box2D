#pragma once

#include "GameObject.h"

class Ball;
class Bonus;

class Block : public QObject, public GameObject {
  Q_OBJECT
public:
  Block(int durability, const QSizeF& sizes, WorldScene* worldScene);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
  QRectF boundingRect() const override;

  void beginContact(GameObject* other, b2Contact* contact) override;

  void reduceDurability(int reducement);
  int durability() const;
signals:
  void damageTaken(int damage);
  void broken(Block* block);

private:
  QRectF m_rect;
  int m_durability;
  bool m_isUbreakable;
};

class AccelBlock : public Block {
  Q_OBJECT
public:
  AccelBlock(float acceleration, int durability, const QSizeF& sizes, WorldScene* worldScene);

  void endContact(GameObject* other, b2Contact* contact) override;

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
private:
  float m_acceleration;
};

struct BonusData {
  Bonus* bonus;
  QPointF fallBonusV;
  QSizeF fallBonusSizes;
};

class BonusBlock : public Block {
  Q_OBJECT
public:
  BonusBlock(const BonusData& bonusData, int durability, const QSizeF& sizes, WorldScene* worldScene);

  void advance(int phase) override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
private:
  BonusData m_bonusData;
  
  void spawnFallingBonus();
};

inline QRectF Block::boundingRect() const {
  return m_rect;
}

inline int Block::durability() const {
  return m_durability;
}
