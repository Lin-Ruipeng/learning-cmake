cmake_minimum_required(VERSION 3.20)
project(main)

# 启用测试
enable_testing()

add_executable(main main.cpp)

# 构建测试程序
add_executable(mytests tests.cpp)
# 添加测试
add_test(NAME my_test COMMAND $<TARGET_FILE:mytests>)
