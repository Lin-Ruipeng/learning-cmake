#include "imu_driver.h"
#include <iostream>
// 不只是导入头文件, 每次调用都要这么写
#ifdef USE_WIN_SERIAL
#include "win_socket.h"
#elif defined(USE_TERMIOS)
#include "unix_socket.h"
#endif

namespace driver {
void test() { std::cout << "This is imu_driver!" << std::endl; }
} // namespace driver
