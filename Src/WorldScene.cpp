#include <box2d/b2_joint.h>
#include "WorldScene.h"
#include "GameCallbacks.h"
#include "Bonus.h"

constexpr int g_velIters = 6;
constexpr int g_posIters = 2;

WorldScene::WorldScene(float timeStep, const QRectF& rect, QObject* parent)
  : QGraphicsScene(rect, parent) {
  m_world = new b2World(b2Vec2(0.0f, 0.0f));

  m_bonusManager = new BonusManager(this);

  m_timeStep = timeStep;
  m_timer = new QTimer;
  m_timer->setInterval(timeStep * 1000);
  QObject::connect(m_timer, &QTimer::timeout, this, &WorldScene::advance);

  m_gameListener = new GameListener;
  m_world->SetContactListener(m_gameListener);
}

void WorldScene::advance() {
  m_bonusManager->update();
  m_world->Step(m_timeStep, g_velIters, g_posIters);
  QGraphicsScene::advance();
  deleteGameObjects();
}

void WorldScene::deleteGameObjects() {
  for (auto gameObj : m_deletionList) {
    m_world->DestroyBody(gameObj->getBody());
    delete gameObj;
  }
  m_deletionList.clear();
}

WorldScene::~WorldScene() {
  delete m_bonusManager;
  delete m_world;
  delete m_timer;
  delete m_gameListener;
}
