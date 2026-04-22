# 根目录的 CMakeLists.txt 包含项目基本信息、find_package，并使用 add_subdirectory 将两个子模块引入
cmake_minimum_required(VERSION 3.20)
project(imu_project)

# 引入标准的安装路径变量 (CMAKE_INSTALL_LIBDIR, CMAKE_INSTALL_INCLUDEDIR 等)
include(GNUInstallDirs)

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

# 可执行文件不需要导出给外部，正常编译即可
add_executable(main "src/main.cpp")
target_link_libraries(main PRIVATE fusion)

# 安装与导出

# 1生成并安装 Export 文件

# 把子目录里收集到的 ImuProjectTargets 篮子，写入到 ImuProjectTargets.cmake 文件中 NAMESPACE
# 极其关键：它会让文件里的 target 自动加上 "ImuProject::" 前缀！
install(
  EXPORT ImuProjectTargets
  NAMESPACE ImuProject::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ImuProject)

# 2处理 Config 模板文件并安装 configure_file 会把 .in 文件复制一份到 build 目录
# (替换掉里面的变量，这里没变量所以原样复制)
configure_file(cmake/ImuProjectConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/ImuProjectConfig.cmake COPYONLY)

# 把生成的 Config 文件安装到和 Targets 文件同一个目录下
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ImuProjectConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ImuProject)
