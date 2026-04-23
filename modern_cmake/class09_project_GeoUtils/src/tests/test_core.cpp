#include <iostream>

#include "point.h"

int main() {
  Point p1{0.0, 0.0};
  Point p2{3.0, 4.0};

  double d = distance(p1, p2);

  if (d > 4.99 && d < 5.1) {
    std::cout << "[PASS] distance test" << std::endl;
    return 0;
  }

  std::cout << "[FAIL] distance test, got " << d << std::endl;
  return 1;
}
