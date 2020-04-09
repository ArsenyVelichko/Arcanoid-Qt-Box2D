#ifndef CONTROLSYSTEM_H
#define CONTROLSYSTEM_H

#include <QEvent.h>
#include <QList>
#include "GameObjects.h"
#include <QTimer>

class Controller : public QObject {
  Q_OBJECT
  QList<int> _activeKeys;
  Carriege* _carr;
  QList<Ball*> _balls;

protected:
  void launchBall();
  bool eventFilter(QObject *obj, QEvent *event) override;

public:
  void advance(const QRectF& gameArea);

  void addCarr(Carriege* carriege) { _carr = carriege; }
  void addBall(Ball* gameBall) { _balls.append(gameBall); }
  void removeBall(Ball* gameBall) { _balls.removeOne(gameBall); }
  bool isEmpty() { return _balls.isEmpty(); }
     
  QList<Ball*>& getBalls() { return _balls; }
  Carriege* getCarr() { return _carr; }

};

#endif // CONTROLSYSTEM_H
