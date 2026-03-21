cmake_minimum_required(VERSION 3.20)

project(myapp)

# 1.1 源文件列表方式
# set(SRC_LIST main.cpp;a.cpp;b.cpp;c.cpp)
# 1.2 源文件搜索方式
# aux_source_directory(. SRC_LIST)
# 1.3 文件搜索方式
file(GLOB SRC_LIST_1 ./${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB SRC_LIST_2 ./src/*.cpp)
set(SRC_LIST ${SRC_LIST_1};${SRC_LIST_2})
# 指定输出路径
# set(EXECUTABLE_OUTPUT_PATH ./build)
# 指定C++版本
set(CMAKE_CXX_STANDARD 11)

# 指定头文件位置
set(INC_DIR_1 ./${CMAKE_CURRENT_SOURCE_DIR}/include)
set(INC_DIR_2 ./include)
set(INC_DIR ${INC_DIR_1};${INC_DIR_2})
include_directories(${INC_DIR})

# 指定生成可执行文件
# add_executable(myApp main.cpp;a.cpp;b.cpp;c.cpp)
add_executable(myApp ${SRC_LIST})

