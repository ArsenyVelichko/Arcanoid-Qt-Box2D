#include "GameObjects.h"
#include <QPainter>
#include <QtMath>
#include "CommonFuncs.h"

PlayerInterface* Block::_scorer = nullptr;

static void AabbVsBallIntersect(const Ball& ball, const QRectF& bbox, HitRecord& record)  {
  QPointF mappedCenter = ball.center() - bbox.center();

  QPointF closestPoint = mappedCenter;
  double halfWidth = bbox.width() / 2;
  double halfHeight = bbox.height() / 2;

  closestPoint.setX(clamp<double>(closestPoint.x(), -halfWidth, halfWidth));
  closestPoint.setY(clamp<double>(closestPoint.y(), -halfHeight, halfHeight));

  bool isInside = false;
  if (closestPoint == mappedCenter) {
    isInside = true;

    if (abs(mappedCenter.x()) > abs(mappedCenter.y())) {
      if (closestPoint.x() > 0) {
        closestPoint.setX(bbox.width() / 2);

      } else {
        closestPoint.setX(-bbox.width() / 2);
      }

    } else {
      if (closestPoint.y() > 0) {
        closestPoint.setY(bbox.height() / 2);

      } else {
        closestPoint.setY(-bbox.height() / 2);
      }
    }
  }

  QVector2D normal(mappedCenter - closestPoint);

  if (isInside) {
    normal = -normal;
  }

  record.normal = normal.normalized();
  record.touchPoint = closestPoint + bbox.center();
}

static double BallVsBallIntersect(const Ball& ball1, const Ball& ball2, HitRecord& record) {
  QVector2D normal = QVector2D(ball1.center() - ball2.center());
  double radiusSum = ball1.radius() + ball2.radius();
  double penetration;

  if (normal.isNull()) {
    penetration = radiusSum;
    normal = QVector2D(-1, 0);

  } else {
    penetration = radiusSum - normal.length();
  }

  record.normal = normal.normalized();
  record.touchPoint = ball1.center() - normal.toPointF() * (ball1.radius() - penetration);

  return penetration / radiusSum;
}

QVector2D Carriege::calcDirect(const Ball& ball, const QPointF& touchPoint) const {

  double halfWidth = width() / 2;
  double centeredX = touchPoint .x() - (left() + right()) / 2;
  double bounceAngle = M_PI_2 * (1 - centeredX / halfWidth * 6. / 9);

  return QVector2D(cos(bounceAngle), sin(bounceAngle));
}

void Carriege::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  double halfWidth = width() / 2;
  double halfHeight = height() / 2;

  QPointF startPoint(-halfWidth, -halfHeight);
  QPointF endPoint(3 * halfWidth, 3 * halfHeight);
  QLinearGradient linearGrad(startPoint, endPoint);

  linearGrad.setColorAt(0, QColor(84, 192, 236));
  linearGrad.setColorAt(1, Qt::white);
  painter->setBrush(linearGrad);
  painter->drawRect(translated(-center()));

  Q_UNUSED(option);
  Q_UNUSED(widget);
}

void Carriege::processBall(Ball& ball) {
  HitRecord record;
  AabbVsBallIntersect(ball, *this, record);

  if (record.touchPoint.y() >= bottom()) {
    QVector2D newDirect = calcDirect(ball, record.touchPoint);
    ball.changeDirect(newDirect);

    ball.changeCenter(record.touchPoint + record.normal.toPointF() * ball.radius());
    ball.setPos(flipY(ball.center()));
  }  
}

Ball::Ball(const QPointF& center, const QVector2D& direct, double normalSpeed,
  double maxSpeed, double radius) {
  _center = center;
  _direct = direct;
  _normalSpeed = normalSpeed;
  _radius = radius;
  _currSpeed = 0;
  _maxSpeed = maxSpeed;
}

Ball::Ball(const Ball& other) {
  _center = other._center;
  _direct = other._direct;
  _normalSpeed = other._normalSpeed;
  _radius = other._radius;
  _currSpeed = other._currSpeed;
  _maxSpeed = other._maxSpeed;
}

void Ball::reflect(const QVector2D normal) {
  _direct = _direct - 2 * QVector2D::dotProduct(_direct, normal) * normal;
}

QRectF Ball::boundingRect() const {
  return QRectF(-_radius, -_radius, 2 * _radius, 2 * _radius);
}

void Ball::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  QPointF startPoint(-_radius, -_radius);
  QPointF endPoint(3 * _radius, 3 * _radius);
  QLinearGradient linearGrad(startPoint, endPoint);

  linearGrad.setColorAt(0, QColor(255, 121, 2));
  linearGrad.setColorAt(1, Qt::white);
  painter->setBrush(linearGrad);
  painter->drawEllipse(QPointF(0, 0), _radius, _radius);

  Q_UNUSED(option);
  Q_UNUSED(widget);
}

void Ball::advance(int phase) {
  if (phase != 0) {
    move();
    setPos(flipY(center()));
  }
}

QPainterPath Ball::shape() const {
  QPainterPath path;
  path.addEllipse(QPointF(0, 0), _radius, _radius);
  return path;
}

void Ball::accelerate(double acceleration) {
  if (_currSpeed + acceleration <= _maxSpeed) {
    _currSpeed += acceleration;
  }
}

void Ball::processBall(Ball& ball) {
  HitRecord record;
  double commonBias = BallVsBallIntersect(*this, ball, record);

  QVector2D speedVec1 = direct() * currSpeed();
  QVector2D speedVec2 = ball.direct() * ball.currSpeed();
  double speedChange = QVector2D::dotProduct(record.normal, -speedVec1 + speedVec2);

  if (speedChange > 0) {
    QVector2D changeAlongNormal = speedChange * record.normal;
    changeDirect((speedVec1 + changeAlongNormal).normalized());
    ball.changeDirect((speedVec2 - changeAlongNormal).normalized());
  }

  double bias1 = commonBias * radius();
  changeCenter(center() + bias1 * record.normal.toPointF());
  setPos(flipY(center()));

  double bias2 = commonBias * ball.radius();
  ball.changeCenter(ball.center() - bias2 * record.normal.toPointF());
  ball.setPos(flipY(ball.center()));
}

void Block::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  QBrush brush;

  switch (_type) {
  case BlockType::UNBREAKABLE:
  { 
    double halfWidth = width() / 2;
    double halfHeight = height() / 2;

    QPointF startPoint(-halfWidth, -halfHeight);
    QPointF endPoint(3 * halfWidth, 3 * halfHeight);
    QLinearGradient linearGrad(startPoint, endPoint);

    linearGrad.setColorAt(0, Qt::black);
    linearGrad.setColorAt(1, Qt::white);

    brush = linearGrad;
    break;
  }
  case BlockType::ACCELERATING:
    brush = QColor(255, 121, 2);
    break;

  default:
    brush = QColor(84, 192, 236);
    break;
  }

  painter->setBrush(brush);
  painter->drawRect(translated(-center()));
  Q_UNUSED(option);
  Q_UNUSED(widget);
}

void Block::processBall(Ball& ball) {
  HitRecord record;

  AabbVsBallIntersect(ball, *this, record);
  ball.reflect(record.normal);
  ball.changeCenter(record.touchPoint + record.normal.toPointF() * ball.radius());
  ball.setPos(flipY(ball.center()));

  if (_type == BlockType::ACCELERATING) {
    ball.accelerate(0.5);
  }

  reduceHp();
  if (isBroken()) {

    if (randomDouble() < 0.17) {
      double sideLen = 15;
      _scorer->bonusHandler()->spawnBonus(scene(), sideLen, center());
    }

    const_cast<BlocksGrid*>(gridPtr)->breakBlock(this);
  }
}

void Block::reduceHp() {
  if (_type != BlockType::UNBREAKABLE) {
    _hp--;
    _scorer->increment();
  }
}

FallingBonus::FallingBonus(double sideLen, const QPointF& center, BonusType type) {
  _type = type;
  _center = center;
  _halfSideLen = sideLen / 2;
}

QRectF FallingBonus::boundingRect() const { 
  return QRectF(-_halfSideLen, -_halfSideLen, 2 * _halfSideLen, 2 * _halfSideLen);
}

void FallingBonus::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  QPointF startPoint(-_halfSideLen, -_halfSideLen);
  QPointF endPoint(2 * _halfSideLen, 2 * _halfSideLen);
  QLinearGradient linearGrad(startPoint, endPoint);

  linearGrad.setColorAt(0, Qt::yellow);
  linearGrad.setColorAt(1, Qt::white);
  painter->setBrush(linearGrad);
  painter->drawEllipse(QPointF(0, 0), _halfSideLen, _halfSideLen);

  Q_UNUSED(option);
  Q_UNUSED(widget);
}

MovingBlock::MovingBlock(const QRectF& box, int health, BlockType type, double speed) :
  Block(box, health, type) {
  _speed = speed;
  _currDirect = Direction::LEFT;
}

void MovingBlock::changeDirect() {
  if (_currDirect == Direction::LEFT) {
    _currDirect = Direction::RIGHT;
  } else {
    _currDirect = Direction::LEFT;
  }
}