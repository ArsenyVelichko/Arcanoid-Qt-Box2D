#include <box2d/b2_contact.h>
#include "GameCallbacks.h"

QVector<GameCallback> GameListener::s_matchList(CallbackName::e_NameCount);
bool GameListener::s_isInit = false;

GameListener::GameListener() {
  if (!s_isInit) {
    s_matchList[CallbackName::e_BeginContact] = &GameObject::beginContact;
    s_matchList[CallbackName::e_EndContact] = &GameObject::endContact;
    s_matchList[CallbackName::e_PreSolve] = &GameObject::preSolve;

    s_isInit = true;
  }
}

void GameListener::matchUp(CallbackName name, b2Contact* contact) {
  b2Body* bodyA = contact->GetFixtureA()->GetBody();
  b2Body* bodyB = contact->GetFixtureB()->GetBody();

  GameObject* gameObjA = (GameObject*)bodyA->GetUserData();
  GameObject* gameObjB = (GameObject*)bodyB->GetUserData();

  if (gameObjA && gameObjB) {
    (gameObjA->*s_matchList[name])(gameObjB, contact);
    (gameObjB->*s_matchList[name])(gameObjA, contact);
  }
}

IsOccupiedQuery::IsOccupiedQuery(const b2Filter& filter) {
  m_filter = filter;
  m_isOccupied = false;
}

bool IsOccupiedQuery::ReportFixture(b2Fixture *fixture) {
  b2Filter fixtureFilter = fixture->GetFilterData();
  bool shouldCollide = (fixtureFilter.maskBits & m_filter.categoryBits) != 0 && 
                       (fixtureFilter.categoryBits & m_filter.maskBits) != 0;

  if (shouldCollide && !fixture->IsSensor()) {
    m_isOccupied = true;
    return false;
  }
  return true;
}