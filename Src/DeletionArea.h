#pragma once

#include <QSet>
#include "GameObject.h"

class DeletionArea : public QObject, public GameObject {
  Q_OBJECT
public:
  DeletionArea(const QSizeF& sizes, WorldScene* worldScene);

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override {}
  void advance(int phase) override;

  void beginContact(GameObject* other, b2Contact* contact) override;

  void addAccepted(const QString& tag);
signals:
  void deleted(GameObject* gameObj);

private:
  QSet<QString> m_acceptedSet;
  QList<GameObject*> m_deletedList;
};

inline QRectF DeletionArea::boundingRect() const {
  return QRectF();
}

inline void DeletionArea::addAccepted(const QString& tag) {
  m_acceptedSet << tag;
}