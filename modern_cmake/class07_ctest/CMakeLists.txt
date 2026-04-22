# 根目录的 CMakeLists.txt 包含项目基本信息、find_package，并使用 add_subdirectory 将两个子模块引入
cmake_minimum_required(VERSION 3.20)
project(imu_project)

# 启用测试!
enable_testing()

# 设置一个条件变量, 默认为ON
option(ENABLE_EXTRA_WARNINGS "Enable -Wall -Wextra for project targets" ON)

find_package(Eigen3 REQUIRED)

# 传入子CMake脚本的文件夹目录
add_subdirectory("src/driver")
add_subdirectory("src/fusion")

if(ENABLE_EXTRA_WARNINGS)
  if(MSVC)
    target_compile_options(driver PRIVATE /W4) # 私有方式传递这个编译选项进去
    target_compile_options(fusion PRIVATE /W4) # 私有方式传递这个编译选项进去
    message(STATUS "ENABLE_EXTRA_WARNINGS is ON, compile options has /W4")
  else()
    target_compile_options(driver PRIVATE -Wall -Wextra) # 私有方式传递这个编译选项进去
    target_compile_options(fusion PRIVATE -Wall -Wextra) # 私有方式传递这个编译选项进去
    message(
      STATUS "ENABLE_EXTRA_WARNINGS is ON, compile options has -Wall -Wextra")
  endif()
endif()

add_executable(main "src/main.cpp")
target_link_libraries(main PRIVATE fusion)
