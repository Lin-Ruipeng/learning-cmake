#include "imu_fusion.h"
#include "imu_driver.h"
#include <Eigen/Dense>
#include <iostream>

namespace fusion {
void test() {
  // 测试Eigen库
  Eigen::Vector2d v{0.1, -0.1};
  std::cout << "This is imu_fusion, v = " << v.transpose() << std::endl;
  driver::test();
};
} // namespace fusion
