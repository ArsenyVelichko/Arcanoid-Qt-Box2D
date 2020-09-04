#include <QPainter>
#include <box2d/b2_polygon_shape.h>
#include "box2d/b2_fixture.h"
#include "Block.h"
#include "Ball.h"
#include "Bonus.h"
#include "WorldScene.h"

using namespace SuppModule;

Block::Block(int durability, const QSizeF& sizes, WorldScene* worldScene)
  : GameObject(worldScene) {
  m_durability = durability;
  m_isUbreakable = durability == 0 ? true : false;

  b2PolygonShape box;
  m_rect = centredAabb(sizes);
  box.SetAsBox(toBox2D(m_rect.right()), toBox2D(m_rect.bottom()));

  b2FixtureDef fd;
  fd.shape = &box;
  fd.friction = 0.0f;
  fd.restitution = 0.0f;

  getBody()->CreateFixture(&fd);
}

void Block::reduceDurability(int reducement) {
  if (m_durability == 0) { return; }

  int damage = qMin(m_durability, reducement);
  m_durability -= damage;
  emit damageTaken(damage);

  if (m_durability == 0) {
    emit broken(this);
    scheduleDeletion();
  }
}

void Block::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  QBrush brush = m_isUbreakable ? Qt::gray : QColor(84, 192, 236);
  painter->setBrush(brush);
  painter->drawRect(boundingRect());
}

void Block::beginContact(GameObject* other, b2Contact* contact) {
  if (other->getTag() == "Ball") {
    reduceDurability(1);
  }
}

AccelBlock::AccelBlock(float acceleration, int durability,
                       const QSizeF& sizes, WorldScene* worldScene)
  : Block(durability, sizes, worldScene) {
  m_acceleration = acceleration;
}

void AccelBlock::endContact(GameObject* other, b2Contact* contact) {
  if (other->getTag() == "Ball") {
    Ball* ball = static_cast<Ball*>(other);
    ball->accelerate(m_acceleration);
  }
  GameObject::endContact(other, contact);
}

void AccelBlock::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  painter->setBrush(QColor(255, 121, 2));
  painter->drawRect(boundingRect());
}
BonusBlock::BonusBlock(const BonusData& bonusData, int durability,
                       const QSizeF& sizes, WorldScene* worldScene)
  : Block(durability, sizes, worldScene) {
  m_bonusData = bonusData;
}

void BonusBlock::spawnFallingBonus() {
  FallingBonus* fallingBonus = new FallingBonus(m_bonusData, getWorldScene());
  fallingBonus->setPos(pos());
}

void BonusBlock::advance(int phase) {
  if (phase && durability() <= 0) {
    spawnFallingBonus();
  }
  GameObject::advance(phase);
}

void BonusBlock::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  painter->setBrush(QColor(173, 255, 47));
  painter->drawRect(boundingRect());
}