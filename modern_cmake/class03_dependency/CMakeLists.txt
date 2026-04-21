# 根目录的 CMakeLists.txt 包含项目基本信息、find_package，并使用 add_subdirectory 将两个子模块引入
cmake_minimum_required(VERSION 3.20)
project(imu_project)

# 本次不使用 find_package(Eigen3 REQUIRED) 而改用网上拉取
include(FetchContent) # 1 加载fetch模块
# 2 手动拉取依赖
FetchContent_Declare(
  eigen # 依赖名, 这个看官方文档, 不要瞎写建议
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.4.0)
# 3 避免重复拉取
FetchContent_MakeAvailable(eigen) # 名字要对应

# 为driver添加线程库依赖
find_package(Threads REQUIRED)

# 传入子CMake脚本的文件夹目录
add_subdirectory("src/driver")
add_subdirectory("src/fusion")

# 需要在创建了target之后再去链接 # 建议移到 src/driver/CMakeLists.txt 中
target_link_libraries(driver PRIVATE Threads::Threads)

add_executable(main "src/main.cpp")
target_link_libraries(main PRIVATE fusion)
