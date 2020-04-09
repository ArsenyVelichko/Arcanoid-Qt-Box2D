#ifndef COMMONFUNCS_H
#define COMMONFUNCS_H

#include <QPointF>

template<typename T>
T clamp(T val, T lo, T hi) {
  return (val < lo) ? lo : (hi < val) ? hi : val;
}

template<typename T>
T sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

QPointF flipY(const QPointF& point);

double randomDouble();

#endif // COMMONFUNCS_H
