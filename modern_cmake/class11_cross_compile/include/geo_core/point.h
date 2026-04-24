#ifndef GEO_CORE_POINT_H
#define GEO_CORE_POINT_H

struct Point {
  double x;
  double y;
};

double distance(const Point& a, const Point& b);

#endif  // GEO_CORE_POINT_H
