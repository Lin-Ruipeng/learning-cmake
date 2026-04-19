# 最小版本, 3.20是最现代的版本
cmake_minimum_required(VERSION 3.20)

# 项目名称
project(ImuProject)

# 因为使用到了Eigen3库, 需要查找
find_package(Eigen3 REQUIRED) # 导入 Eigen3::Eigen 目标

# message("pwd = ${CMAKE_CURRENT_SOURCE_DIR}")

# driver库
add_library(driver STATIC) # 添加构建目标
# 添加源文件, 私有表示只有这个库编译的时候需要这个cpp文件
target_sources(driver PRIVATE driver/imu_driver.cpp)
# 添加头文件路径
target_include_directories(driver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/driver)
# 假设driver库使用了17的特性, 并且希望传递给后续的目标(所以用公开)
target_compile_features(driver PUBLIC cxx_std_17)
# 假设driver使用了serial_port: target_link_libraries(driver PRIVATE
# SerialPort::SerialPort)

# fusion库, 依赖于Eigen3
add_library(fusion STATIC)
# 添加源文件, 私有表示只有这个库编译的时候需要这个cpp文件
target_sources(fusion PRIVATE fusion/imu_fusion.cpp)
target_include_directories(fusion PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/fusion)
# 因为用到了 注意这里是链接库!
target_link_libraries(
  fusion
  PUBLIC Eigen3::Eigen # 因为别人调用时也需要这个库才好交互
  PRIVATE driver) # drive只有fusion用到了, 不需要对外暴露
# 这里用公开是因为需要把这种头文件依赖向外传递, 其他人不包含这俩依赖用不了这个库

# main在依赖了fusion的时候就会因为这个公开,而导入driver和Eigen库

# 可执行文件
add_executable(main main.cpp)
target_link_libraries(main PRIVATE fusion)
# 只需链接 fusion, 它会自动传递 driver 和 Eigen 的包含路径及链接信息

# 所以解释了为什么不需要写：target_include_directories(main PUBLIC
# ${CMAKE_CURRENT_SOURCE_DIR})
