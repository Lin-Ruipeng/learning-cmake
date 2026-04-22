# src/fusion/CMakeLists.txt 构建 fusion 静态库。要求它把 include/fusion 作为 PUBLIC
# 路径暴露，并正确链接 Eigen 和 driver。

# 构建静态库
add_library(fusion STATIC)
target_sources(fusion PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imu_fusion.cpp")
# target_include_directories(fusion PUBLIC "../../include/fusion")
target_include_directories(fusion PUBLIC "${CMAKE_SOURCE_DIR}/include/fusion")

target_link_libraries(
  fusion
  PUBLIC Eigen3::Eigen
  PRIVATE driver)

# 增加编译的特性需求, 且传导这个需求到下游
target_compile_features(fusion PUBLIC cxx_std_17)
