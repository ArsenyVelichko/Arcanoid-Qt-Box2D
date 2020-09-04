#include <QPainter>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "Bonus.h"
#include "WorldScene.h"
#include "Ball.h"
#include "Block.h"
#include "Paddle.h"

using namespace SuppModule;

constexpr float g_newBallRot = b2_pi / 2.0f;
constexpr int g_bottomDurability = 1;

BonusManager::BonusManager(WorldScene* worldScene) {
  m_worldScene = worldScene;
}

void BonusManager::update() {
  for (int i = 0; i < m_bonusList.size(); i++) {

    if (m_bonusList[i]->m_isDeleted) {
      delete m_bonusList[i];
      m_bonusList.removeAt(i);
      continue;
    }

    if (m_bonusList[i]->m_isActivated) {
      m_bonusList[i]->update();
    }
  }
}

BonusManager::~BonusManager() {
  for (auto bonus : m_bonusList) {
    delete bonus;
  }
}

Bonus::Bonus(BonusManager* manager) {
  m_manager = manager;
  manager->addBonus(this);
  m_isActivated = false;
  m_isDeleted = false;
}

DoubleBall::DoubleBall(QList<Ball*>&& ballList, BonusManager* manager)
  : m_ballList(ballList), Bonus(manager) {}

void DoubleBall::activate() {
  if (m_ballList.isEmpty()) { return; }

  Ball* sample = m_ballList[randomFloat() * m_ballList.size()];
  b2Body* sampleBody = sample->getBody();

  b2Vec2 newBallV = sampleBody->GetLinearVelocity();
  newBallV = b2Mul(b2Rot(g_newBallRot), newBallV);

  float radius = sample->getRadius();
  WorldScene* worldScene = sample->getWorldScene();
  Ball* newBall = new Ball(radius, fromBox2D(newBallV), worldScene);
  newBall->setPos(sample->scenePos());
  m_ballList.append(newBall);

  m_isDeleted = true;
}

AddBottom::AddBottom(const QPointF& pos, const QSizeF& sizes, BonusManager* manager)
  : Bonus(manager) {
  m_pos = pos;
  m_sizes = sizes;
}

void AddBottom::activate() {
  WorldScene* worldScene = m_manager->getWorldScene();
  Block* block = new Block(g_bottomDurability, m_sizes, worldScene);
  block->setPos(m_pos);

  m_isDeleted = true;
}

AccelerateBall::AccelerateBall(float acceleration, QList<Ball*>&& ballList,
                               BonusManager* manager)
  : m_ballList(ballList), Bonus(manager) {
  m_acceleration = acceleration;
}

void AccelerateBall::activate() {
  if (m_ballList.isEmpty()) { return; }

  Ball* ball = m_ballList[randomFloat() * m_ballList.size()];
  ball->accelerate(m_acceleration);

  m_isDeleted = true;
}

TmpBonus::TmpBonus(float duration, BonusManager* manager) : Bonus(manager) {
  m_remainingTime = duration;
  m_timeStep = manager->getWorldScene()->getTimeStep();
}

void TmpBonus::activate() {
  m_isActivated = true;
}

void TmpBonus::update() {
  m_remainingTime -= m_timeStep;
  if (m_remainingTime <= 0) {
    deactivate();
    m_isDeleted = true;
  }
}

RandomDirect::RandomDirect(float probability, QList<Ball*>&& ballList,
                           float duration, BonusManager* manager)
  : m_ballList(ballList), TmpBonus(duration, manager) {
  m_probability = probability;
}


void RandomDirect::update() {
  if (randomFloat() < m_probability) {
    changeDirect();
  }
  TmpBonus::update();
}

void RandomDirect::changeDirect() const {
  if (m_ballList.isEmpty()) { return; }

  Ball* ball = m_ballList[randomFloat() * m_ballList.size()];
  b2Body* ballBody = ball->getBody();

  if (ballBody->IsEnabled()) {
    ballBody->SetLinearVelocity(randomUnitVec());
    ball->setSpeedToConst();
  }
}

StickyPaddle::StickyPaddle(Paddle* paddle, float duration, BonusManager* manager)
  : TmpBonus(duration, manager) {
  m_paddle = paddle;
}

void StickyPaddle::activate() {
  m_paddle->setSticky(true);
  TmpBonus::activate();
}

void StickyPaddle::deactivate() {
  m_paddle->setSticky(false);
}

ExpandPaddle::ExpandPaddle(Paddle* paddle, float expansion, BonusManager* manager)
  : Bonus(manager) {
  m_paddle = paddle;
  m_expansion = expansion;
}

void ExpandPaddle::activate() {
  m_paddle->expand(m_expansion);
  m_isDeleted = true;
}

FallingBonus::FallingBonus(const BonusData& bonusData, WorldScene* worldScene)
  : GameObject(worldScene) {
  m_isCatched = false;
  m_bonus = bonusData.bonus;

  b2Body* body = getBody();
  body->SetType(b2_dynamicBody);
  body->SetLinearVelocity(toBox2D(bonusData.fallBonusV));
  setConstSpeed(true);

  b2PolygonShape box;
  m_rect = centredAabb(bonusData.fallBonusSizes);
  box.SetAsBox(toBox2D(m_rect.right()), toBox2D(m_rect.bottom()));

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &box;
  fixtureDef.isSensor = true;
  body->CreateFixture(&fixtureDef);
}

void FallingBonus::advance(int phase) {
  if (phase && m_isCatched) {
    m_bonus->activate();
    scheduleDeletion();
  }
  GameObject::advance(phase);
}

void FallingBonus::beginContact(GameObject* other, b2Contact* contact) {
  if (other->getTag() == "Paddle") {
    m_isCatched = true;
  }
}

void FallingBonus::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  painter->setBrush(QColor(255, 255, 0));
  painter->drawRect(m_rect);
}

QString FallingBonus::getTag() const {
  return "FallingBonus";
}