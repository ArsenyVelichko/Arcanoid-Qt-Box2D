#pragma once

#include <QObject>
#include "WorldScene.h"
#include "Ball.h"
#include "PlayerInterface.h"
#include "Controller.h"
#include "Paddle.h"
#include "Bonus.h"

class GameMode : public QObject {
  Q_OBJECT
public:
  GameMode(const QString& levelName, WorldScene* worldScene);
  ~GameMode();

private slots:
  void countBlockDestruct();
  void catchBallDeletion(GameObject* gameObj);

private:
  WorldScene* m_worldScene;

  QList<Ball*> m_ballList;
  Scorer* m_scorer;
  HealthIndicator* m_hpIndicator;
  Controller<Paddle>* m_controller;
  int m_breakBlockCount;

  void createBall();
  void loadLevel(const QString& levelName);
  void createBlock(const QStringList& params);
  Bonus* createBonus(char type);

  void countWin();
  void countLose();
};

