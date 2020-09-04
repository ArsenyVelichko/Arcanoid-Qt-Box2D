#pragma once

#include <QTimer>
#include <QGraphicsScene>
#include <Box2D/b2_world.h>

class GameObject;
class BonusManager;
class GameListener;

class WorldScene : public QGraphicsScene {
  Q_OBJECT
public:
  WorldScene(float timeStep, const QRectF& rect, QObject* parent = nullptr);
  ~WorldScene();

  void start();
  void stop();
  BonusManager* getBonusManager() const;
  float getTimeStep() const;

private slots:
  void advance();

private:
  friend class GameObject;

  b2World* m_world;
  BonusManager* m_bonusManager;
  GameListener* m_gameListener;

  QTimer* m_timer;
  float m_timeStep;

  QList<GameObject*> m_deletionList;
  void scheduleDeletion(GameObject* gameObj);
  void deleteGameObjects();
};

inline void WorldScene::start() {
  m_timer->start();
}

inline void WorldScene::stop() {
  m_timer->stop();
}

inline BonusManager* WorldScene::getBonusManager() const {
  return m_bonusManager;
}

inline void WorldScene::scheduleDeletion(GameObject* gameObj) {
  m_deletionList.append(gameObj);
}

inline float WorldScene::getTimeStep() const {
  return m_timeStep;
}
