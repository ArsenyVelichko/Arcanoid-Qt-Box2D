#pragma once

#include <box2d/b2_fixture.h>
#include <box2d/b2_world_callbacks.h>
#include "GameObject.h"

using GameCallback = void (GameObject::*)(GameObject* other, b2Contact *contact);

class GameListener : public b2ContactListener {
public:
  GameListener();

  void BeginContact(b2Contact *contact) override;
  void EndContact(b2Contact *contact) override;
  void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

  enum CallbackName {
    e_BeginContact,
    e_EndContact,
    e_PreSolve,
    e_NameCount,
  };

private:
  static QVector<GameCallback> s_matchList;
  static bool s_isInit;

  void matchUp(CallbackName name, b2Contact* contact);
};

class IsOccupiedQuery : public b2QueryCallback {
public:
  IsOccupiedQuery(const b2Filter& filter = b2Filter());

  bool ReportFixture(b2Fixture *fixture);
  bool isOccupied() const;

private:
  bool m_isOccupied;
  b2Filter m_filter;
};

inline bool IsOccupiedQuery::isOccupied() const {
  return m_isOccupied;
}

inline void GameListener::BeginContact(b2Contact *contact) {
  matchUp(CallbackName::e_BeginContact, contact);
}

inline void GameListener::EndContact(b2Contact *contact) {
  matchUp(CallbackName::e_EndContact, contact);
}

inline void GameListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
  matchUp(CallbackName::e_PreSolve, contact);
}