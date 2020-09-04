#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>
#include "DeletionArea.h"

using namespace SuppModule;

DeletionArea::DeletionArea(const QSizeF& sizes, WorldScene* worldScene)
  : GameObject(worldScene) {

  b2PolygonShape box;
  float hWidth = sizes.width() / 2;
  float hHeight = sizes.height() / 2;
  box.SetAsBox(toBox2D(hWidth), toBox2D(hHeight));

  b2FixtureDef fd;
  fd.shape = &box;
  fd.isSensor = true;

  getBody()->CreateFixture(&fd);
}

void DeletionArea::beginContact(GameObject* other, b2Contact* contact) {
  if (m_acceptedSet.contains(other->getTag())) {
    m_deletedList.append(other);
    other->scheduleDeletion();
  }
}

void DeletionArea::advance(int phase) {
  if (phase) {
    for (auto gameObj : m_deletedList) {
      emit deleted(gameObj);
    }
    m_deletedList.clear();
  }
}