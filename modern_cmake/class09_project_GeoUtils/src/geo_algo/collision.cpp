#include "collision.h"

bool rect_intersect(const Rect& a, const Rect& b) {
  if (a.max.x < b.min.x || b.max.x < a.min.x) return false;
  if (a.max.y < b.min.y || b.max.y < a.min.y) return false;
  return true;
}
