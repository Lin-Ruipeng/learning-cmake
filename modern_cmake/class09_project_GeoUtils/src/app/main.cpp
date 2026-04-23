#include <iostream>

#include "collision.h"
#include "point.h"

int main() {
  Point p1{0.0, 0.0};
  Point p2{3.0, 4.0};
  std::cout << "Distace: " << distance(p1, p2) << std::endl;

  Rect a{{0, 0}, {2, 2}};
  Rect b{{1, 1}, {3, 3}};
  std::cout << "Intersect: " << (rect_intersect(a, b) ? "true" : "false") << std::endl;

  return 0;
}
