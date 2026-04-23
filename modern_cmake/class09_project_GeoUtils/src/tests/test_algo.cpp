#include "collision.h"
#include <iostream>

int main() {
  Rect a{{0, 0}, {2, 2}};
  Rect b{{1, 1}, {3, 3}};
  Rect c{{10, 10}, {12, 12}};

  if (rect_intersect(a, b) && !rect_intersect(a, c)) {
    std::cout << "[PASS] collision test" << std::endl;
    return 0;
  }

  std::cout << "[FAIL] collision test" << std::endl;
  return 1;

}

