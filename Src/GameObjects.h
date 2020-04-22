#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <QGraphicsItem>
#include <QVector2D>
#include "ScoreAndBonuses.h"
#include "BlocksGrid.h"

struct HitRecord {
  QPointF touchPoint;
  QVector2D normal;
};

class Ball;

class Hittable : public QGraphicsItem {
public:
  virtual void processBall(Ball& ball) = 0;
};

class Ball : public Hittable {
  QPointF _center;
  QVector2D _direct;
  double _normalSpeed;
  double _currSpeed;
  double _radius;
  double _maxSpeed;

protected:
  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
  QPointF center() const { return _center; }
  double radius() const { return _radius; }
  QVector2D direct() const { return _direct; }
  double currSpeed() const { return _currSpeed; }

  bool reflect(const QVector2D normal);
  void setSpeedToNormal() { _currSpeed = _normalSpeed; }
  bool isMoving() { return _currSpeed != 0; }
  void move() { _center += _direct.toPointF() * _currSpeed; }

  void changeDirect(const QVector2D& newDirect) { _direct = newDirect; };
  void changeCenter(const QPointF& newCenter) { _center = newCenter; };
  void accelerate(double acceleration);

  Ball(const QPointF& center, const QVector2D& direct, double normalSpeed,
    double maxSpeed, double radius);
  Ball(const Ball& other);

  QPainterPath shape() const override;
  virtual void processBall(Ball& ball);
};

class Carriege : public QRectF, public Hittable {
  double _speed;

  QVector2D calcDirect(const Ball& ball, const QPointF& touchPoint) const;

protected:
  QRectF boundingRect() const { return translated(-center()); }
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
  Carriege(const QRectF& box, double speed) : QRectF(box), _speed(speed) {}

  double speed() const { return _speed; }

  virtual void processBall(Ball& ball);
};
  
enum class BlockType {
  UNBREAKABLE,
  ACCELERATING,
  DEFAULT,
};

class Block : public QRectF, public Hittable {
  int _hp;
  BlockType _type;
  static PlayerInterface* _scorer;

  friend Block* BlocksGrid::CreateBlock(size_t i, size_t j, int health, BlockType type);

protected:
  const BlocksGrid* gridPtr = nullptr;
  int indexInGrid;

  QRectF boundingRect() const { return translated(-center()); }
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  Block(const QRectF& box, int health, BlockType type) : QRectF(box), _hp(health), _type(type) {}

public:
  static void setScorer(PlayerInterface* scorerPtr) { _scorer = scorerPtr; }
  void reduceHp();
  bool isBroken() const { return _hp <= 0; }
  int index() const { return indexInGrid; }

  virtual void processBall(Ball& ball);
};

class FallingBonus : public QGraphicsItem {
  BonusType _type;
  double _halfSideLen;
  QPointF _center;
  const int _speed = 2;

protected:
  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  QPainterPath shape() const override { return QPainterPath(); }

public:
  FallingBonus(double sideLen, const QPointF& center, BonusType type);

  void move() { _center -= QPointF(0, 2); }

  bool isUnder(double horizBound) const { return _center.y() + _halfSideLen < horizBound; }
  BonusType bonusType() const { return _type; }
  QPointF center() const { return _center; }
  double halfLen() const { return _halfSideLen; }
};

class MovingBlock : public Block {
public:

  enum class Direction {
    RIGHT,
    LEFT,
  };

  Direction direct() { return _currDirect; }
  double speed() { return _speed; }

  void changeDirect();
  void changeIndex(int newIndex) { indexInGrid = newIndex; }

private:
   
  Direction _currDirect;
  double _speed;

  MovingBlock(const QRectF& box, int health, BlockType type, double speed);

  friend MovingBlock* BlocksGrid::CreateMovingBlock(const QList<Ball*>& balls);
};

#endif //GAMEOBJECTS_H