#ifndef GEO_ALGO_COLLISION_H
#define GEO_ALGO_COLLISION_H

#include "point.h"

struct Rect {
  Point min;
  Point max;
};

bool rect_intersect(const Rect& a, const Rect& b);

#endif  // GEO_ALGO_COLLISION_H
