# 根目录的 CMakeLists.txt 包含项目基本信息、find_package，并使用 add_subdirectory 将两个子模块引入
cmake_minimum_required(VERSION 3.20)
project(imu_project)

find_package(Eigen3 REQUIRED)

# 传入子CMake脚本的文件夹目录
add_subdirectory("src/driver")
add_subdirectory("src/fusion")

# message("父节点脚本当前目录: ${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(main "src/main.cpp")
target_link_libraries(main PRIVATE fusion)
