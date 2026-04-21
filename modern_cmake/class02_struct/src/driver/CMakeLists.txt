# src/driver/CMakeLists.txt 构建 driver 静态库。要求它把 include/driver 作为 PUBLIC 路径暴露出去。

# 子目录不要写 cmake_minimum_required(VERSION 3.20) project(imu_driver)

# 构建静态库
add_library(driver STATIC)
target_sources(driver PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imu_driver.cpp")
# target_include_directories(driver PUBLIC "../../include/driver")
target_include_directories(driver PUBLIC "${CMAKE_SOURCE_DIR}/include/driver")
# 假设driver使用了serial_port: # 可惜实际上我的系统里没有这个库

# target_link_libraries(driver PRIVATE SerialPort::SerialPort)
