#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QGraphicsScene>
#include "ControlSystem.h"
#include "BlocksGrid.h"
#include "ScoreAndBonuses.h"

class GameBoard : public QGraphicsScene {
  BlocksGrid* _grid;
  Controller* _controller;
  QTimer* _inGameTimer;
  PlayerInterface* _scorer;
  BonusHandler* _bonusH;
  QRectF* _gameArea;

  void restoreBall();
  bool isOutOfBounds(Ball* ball, HitRecord& record);
  QGraphicsItem* ClosestCollision(const Ball* ball) const;

  friend void BonusHandler::activateBonus(GameBoard* board, BonusType type);
  friend void BonusHandler::deactivateBonus(GameBoard* board, BonusType type);
  void advance();
  ~GameBoard();

public:
  GameBoard(const QRectF& sceneRect, const QRectF& gameArea, int framesPerSec);
  
  bool CreateCarriege(const QVector2D& sizes, double heightOfLane, double speed);
  bool CreateBall(double radius, double normalSpeed, double maxSpeed);
  void CreateBlocksGrid(const QRectF& box, std::istream& source);
};

#endif //GAMEBOARD_H