#include <QFont>
#include <QPainter>
#include "PlayerInterface.h"

Scorer::Scorer() {
  m_score = 0;
  setPlainText("Score:" + QString::number(m_score));
  setFont(QFont("Helvetica [Cronyx]", 16, QFont::Bold));
}

void Scorer::increaseScore(int i) {
  m_score += i;
  setPlainText("Score: " + QString::number(m_score));
}

HealthIndicator::HealthIndicator(int healthPoints) {
  m_hp = healthPoints;
  setFont(QFont("Helvetica [Cronyx]", 16, QFont::Bold));
  setPlainText("Health:");

  QPixmap rawTexture(":/Textures/Heart");
  float height = boundingRect().height();
  m_heartTexture = rawTexture.scaledToHeight(height, Qt::SmoothTransformation);
}

QRectF HealthIndicator::boundingRect() const {
  QRectF textRect = QGraphicsTextItem::boundingRect();
  float heartsWidth = m_hp * m_heartTexture.width();
  return textRect.adjusted(0.0, 0.0, heartsWidth, 0.0);
}

void HealthIndicator::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  QGraphicsTextItem::paint(painter, option, widget);

  QRectF textRect = QGraphicsTextItem::boundingRect();
  QPointF origin = textRect.topRight();
  float bias = m_heartTexture.width();
  for (int i = 0; i < m_hp; i++) {
    painter->drawPixmap(origin, m_heartTexture);
    origin.setX(origin.x() + bias);
  }
}