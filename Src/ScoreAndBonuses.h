#ifndef SCORESYSTEM_H
#define SCORESYSTEM_H

#include <QLCDNumber>
#include <QPixmap>
#include <QGraphicsItem>

class Carriege;
class FallingBonus;
class GameBoard;

enum class BonusType {
  DOUBLE_BALL,
  MOVING_BLOCK,
};

struct ActiveBonus {
  BonusType type;
  int duration;
  QList<QGraphicsItem*> relatedItems;

  ActiveBonus(BonusType type, int duration) : type(type), duration(duration) {}
};

class BonusHandler {
  QList<FallingBonus*> _fallingBonuses;
  QList<ActiveBonus*> _activeBonuses;

public:
  void advance(GameBoard* board, double lowerBound, const Carriege* carr);
  void spawnBonus(QGraphicsScene* scene, double sideLen, const QPointF& center);

  void activateBonus(GameBoard* board, BonusType type);
  void deactivateBonus(GameBoard* board, BonusType type);
};

class PlayerInterface {
  int _score;
  QLCDNumber* _lcd;
  BonusHandler* _bonusH;
  QList<QGraphicsPixmapItem*> _hearts;

  QGraphicsPixmapItem* victoryImgItem;
  QGraphicsPixmapItem* defeatImgItem;

public:
  PlayerInterface();
  ~PlayerInterface();

  void increment();

  void display() const { _lcd->display(_score); }
  void reduceHeart(GameBoard* board);
  void createHearts(GameBoard* board, const QPointF& pos);
  void createCounter(GameBoard* board, const QPointF& pos);
  bool isHeartsLeft() { return !_hearts.isEmpty(); }

  void createEndImages(GameBoard* board, const QPointF& pos);
  void countVictory() { victoryImgItem->setVisible(true); }
  void countLose() { defeatImgItem->setVisible(true); }

  BonusHandler* bonusHandler() { return _bonusH; }
};


#endif //SCORESYSTEM_H

