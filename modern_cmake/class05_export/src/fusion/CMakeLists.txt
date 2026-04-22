# src/fusion/CMakeLists.txt 构建 fusion 静态库。 要求它把 include/fusion 作为 PUBLIC
# 路径暴露，并正确链接 Eigen 和 driver。

# 构建静态库
add_library(fusion STATIC)
target_sources(fusion PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imu_fusion.cpp")
target_include_directories(
  fusion PUBLIC "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/fusion>"
                "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/fusion>")

target_link_libraries(
  fusion
  PUBLIC Eigen3::Eigen
  PRIVATE driver)

# 安装和导出

# 1创建别名
add_library(ImuProject::fusion ALIAS fusion)
# 2安装头文件
install(FILES "${CMAKE_SOURCE_DIR}/include/fusion/imu_fusion.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/fusion")
# 3Target安装导出
install(
  TARGETS fusion
  EXPORT ImuProjectTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
