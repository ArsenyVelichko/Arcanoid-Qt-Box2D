#pragma once
#include <QObject>
#include <QHash>
#include <QKeyEvent>

template<typename PawnType>
class Controller : public QObject {
private:
  using MemberFunc = void (PawnType::*)();

  struct BindingInfo {
    MemberFunc onPress;
    MemberFunc onRelease;
    MemberFunc onRepeat;
  };

  PawnType* m_pawn;
  QHash<int, BindingInfo> m_bindings;

protected:
  bool eventFilter(QObject* obj, QEvent* event) override {
    QEvent::Type type = event->type();
    if (type == QEvent::KeyPress || type == QEvent::KeyRelease) {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
      int key = keyEvent->key();

      if (m_bindings.contains(key)) {
        MemberFunc response = nullptr;

        if (keyEvent->isAutoRepeat()) {
          response = m_bindings[key].onRepeat;

        } else if (type == QEvent::KeyPress) {
          response = m_bindings[key].onPress;

        } else {
          response = m_bindings[key].onRelease;
        }

        if (response) {
          (m_pawn->*response)();
        }
      }
    }
    return false;
  }

public:
  Controller(PawnType* pawn) : m_pawn(pawn) {}

  PawnType* getPawn() const {
    return m_pawn;
  }

  void bindKey(int key, MemberFunc onPress, MemberFunc onRelease, MemberFunc onRepeat) {
    BindingInfo bi;
    bi.onPress = onPress;
    bi.onRelease = onRelease;
    bi.onRepeat = onRepeat;

    m_bindings[key] = bi;
  }
};