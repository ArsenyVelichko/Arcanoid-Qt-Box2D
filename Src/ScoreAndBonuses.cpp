#include "GameObjects.h"
#include "CommonFuncs.h"
#include "ScoreAndBonuses.h"
#include "GameBoard.h"
#include <QGraphicsProxyWidget>

PlayerInterface::PlayerInterface() {
  _score = 0;
  _lcd = new QLCDNumber;
  _bonusH = new BonusHandler;
}

void PlayerInterface::createEndImages(GameBoard* board, const QPointF& pos) {
  QPixmap victoryImage(":/Arcanoid/VictoryImage.png");
  victoryImgItem = board->addPixmap(victoryImage);
  victoryImgItem->setVisible(false);
  victoryImgItem->setZValue(2);

  QRectF bbox = victoryImgItem->boundingRect();
  QPointF topLeft(-bbox.width() / 2, -bbox.height() / 2);
  victoryImgItem->setPos(pos + topLeft);
  
  QPixmap defeatImage(":/Arcanoid/DefeatImage.png");
  defeatImgItem = board->addPixmap(defeatImage);
  defeatImgItem->setVisible(false);
  defeatImgItem->setZValue(2);

  bbox = defeatImgItem->boundingRect();
  topLeft = QPointF(-bbox.width() / 2, -bbox.height() / 2);
  defeatImgItem->setPos(pos + topLeft);
}

void PlayerInterface::createHearts(GameBoard* board, const QPointF& pos) {
  QPointF currPos = pos;

  for (int i = 0; i < 3; i++) {
    QPixmap heartImage(":/Arcanoid/heart.jpg");
    QGraphicsPixmapItem* heartItem = board->addPixmap(heartImage);

    heartItem->setPos(currPos);
    _hearts.append(heartItem);
    currPos += QPointF(heartImage.width(), 0);
  }
}

void PlayerInterface::createCounter(GameBoard* board, const QPointF& pos) {
  QGraphicsProxyWidget* lcdProxy = board->addWidget(_lcd);
  lcdProxy->setPos(pos);
}

void PlayerInterface::increment() {
  _score++;
  _lcd->display(_score);
}

void PlayerInterface::reduceHeart(GameBoard* board) {
  QGraphicsPixmapItem* lastHeart = _hearts.takeLast();
  board->removeItem(lastHeart);
  delete lastHeart;
}

PlayerInterface::~PlayerInterface() {
  delete _bonusH;
}

void BonusHandler::spawnBonus(QGraphicsScene* scene, double sideLen, const QPointF& center) {
  BonusType type;

  if (randomDouble() > 0.5) {
    type = BonusType::MOVING_BLOCK;

  } else {
    type = BonusType::DOUBLE_BALL;
  }

  FallingBonus* bonus = new FallingBonus(sideLen, center, type);

  scene->addItem(bonus);
  bonus->setPos(flipY(center));
  bonus->setZValue(1);

  _fallingBonuses.append(bonus);
}

void BonusHandler::activateBonus(GameBoard* board, BonusType type) {
  switch (type) {
  case BonusType::DOUBLE_BALL:
  {
    QList<Ball*>& ballsList = board->_controller->getBalls();
    int ballIndex(randomDouble() * ballsList.size());
    Ball* newBall = new Ball(*ballsList[ballIndex]);

    board->addItem(newBall);
    board->_controller->addBall(newBall);
    newBall->setPos(flipY(newBall->center()));
    newBall->changeDirect(QVector2D(-newBall->direct().x(), newBall->direct().y()));

    break;
  }
  case BonusType::MOVING_BLOCK:
  {
    QList<Ball*>& balls = board->_controller->getBalls();
    MovingBlock* mBlock = board->_grid->CreateMovingBlock(balls);

    if (mBlock) {
      board->addItem(mBlock);
      mBlock->setPos(flipY(mBlock->center()));
    }

    break;
  }
  default:
    break;
  }
}

void BonusHandler::advance(GameBoard* board, double lowerBound, const Carriege* carr) {
  for (FallingBonus* bonus : _fallingBonuses) {
    bonus->move();
    bonus->setPos(flipY(bonus->center()));

    QPainterPath bonusPath;
    bonusPath.addEllipse(bonus->center(), bonus->halfLen(), bonus->halfLen());

    if (bonusPath.intersects(*carr)) {
      activateBonus(board, bonus->bonusType());
      _fallingBonuses.removeOne(bonus);
      board->removeItem(bonus);
      delete bonus;

    } else if (bonus->isUnder(lowerBound)) {
      _fallingBonuses.removeOne(bonus);
      board->removeItem(bonus);
      delete bonus;
    }
  }
}
