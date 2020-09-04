#pragma once

#include <QRectF>
#include <Box2d/b2_collision.h>

namespace SuppModule {
  constexpr float g_box2DScale = 40.0f;

  inline float fromBox2D(float x) { return x * g_box2DScale; }
  inline float toBox2D(float  x) { return x / g_box2DScale; }

  inline QPointF fromBox2D(const b2Vec2& vec) {
    return QPointF(fromBox2D(vec.x), fromBox2D(vec.y));
  }

  inline b2Vec2 toBox2D(const QPointF& p) {
    return b2Vec2(toBox2D(p.x()), toBox2D(p.y()));
  }

  inline QRectF fromBox2D(const b2AABB& aabb) {
    return QRectF(fromBox2D(aabb.lowerBound), fromBox2D(aabb.upperBound));
  }

  inline b2AABB toBox2D(const QRectF& rect) {
    b2AABB aabb;
    aabb.lowerBound = toBox2D(rect.topLeft());
    aabb.upperBound = toBox2D(rect.bottomRight());
    return aabb;
  }

  inline float randomFloat() {
    return rand() / (RAND_MAX + 1.0f);
  }

  inline b2Vec2 randomUnitVec() {
    float angle = randomFloat() * 2 * b2_pi;
    return b2Vec2(cos(angle), sin(angle));
  }

  inline QPointF minPoint(const QPointF& a, const QPointF& b) {
    return QPointF(qMin(a.x(), b.x()), qMin(a.y(), b.y()));
  }

  inline QPointF maxPoint(const QPointF& a, const QPointF& b) {
    return QPointF(qMax(a.x(), b.x()), qMax(a.y(), b.y()));
  }

  inline QRectF centredAabb(const QSizeF& sizes) {
    float hWidth = sizes.width() / 2;
    float hHeight = sizes.height() / 2;
    return QRectF(QPointF(-hWidth, -hHeight), sizes);
  }
}