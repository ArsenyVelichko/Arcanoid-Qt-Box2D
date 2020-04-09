#include "CommonFuncs.h"

QPointF flipY(const QPointF& point) {
 QPointF res = point;
 res.setY(-res.y());
 return res;
}

double randomDouble() {
  return rand() / (RAND_MAX + 1.0);
}
