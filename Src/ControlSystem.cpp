#include "ControlSystem.h"
#include "CommonFuncs.h"

bool Controller::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    int key = static_cast<QKeyEvent *>(event)->key();

    if (key == Qt::Key_Left || key == Qt::Key_Right) {
      if (!_activeKeys.contains(key)) {
        _activeKeys.push_back(key);
      }

    } else if (key == Qt::Key_Space) {
      launchBall();
    }

    return true;
  } else if (event->type() == QEvent::KeyRelease) {
    int key = static_cast<QKeyEvent *>(event)->key();

    if (key == Qt::Key_Left || key == Qt::Key_Right) {
      _activeKeys.removeOne(key);
    }
    return true;
  }
  return false;
}

void Controller::advance(const QRectF& gameArea) {
  if (!_activeKeys.isEmpty()) {
    int key = _activeKeys.last();
    double halfWidth = gameArea.width() / 2;

    if (_carr->left() >= -halfWidth && _carr->right() <= halfWidth) {

      double speed = _carr->speed();
      if (key == Qt::Key_Left) {
        speed *= -1;
      }

      if (_carr->left() + speed < -halfWidth) {
        speed = -halfWidth - _carr->left();

      } else if (_carr->right() + speed > halfWidth) {
        speed = halfWidth - _carr->right();
      }

      _carr->moveLeft(_carr->left() + speed);
      _carr->setPos(flipY(_carr->center()));

      for (Ball* ball : _balls) {
        if (!ball->isMoving()) {
          QPointF ballCenter = ball->center();
          ball->changeCenter(QPointF(ballCenter.x() + speed, ballCenter.y()));
          ball->setPos(flipY(ball->center()));
        }
      }
    }
  }
}

void Controller::launchBall() {
  for (Ball* ball : _balls) {
    if (!ball->isMoving()) {
      ball->setSpeedToNormal();
    }
  }
}
