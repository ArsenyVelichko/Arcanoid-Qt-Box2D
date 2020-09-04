#pragma once

#include <QGraphicsTextItem>

class Scorer : public QGraphicsTextItem {
  Q_OBJECT
public:
  Scorer();

public slots:
  void increaseScore(int i);

private:
  int m_score;
};

class HealthIndicator : public QGraphicsTextItem {
public:
  HealthIndicator(int healthPoints);

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  void reduceHealth(int reducement);
  int getHp() const;
private:
  QPixmap m_heartTexture;
  int m_hp;
};

inline int HealthIndicator::getHp() const {
  return m_hp;
}

inline void HealthIndicator::reduceHealth(int reducement) {
  m_hp -= qMin(m_hp, reducement);
}
