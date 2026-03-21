# 1. CMake 最小版本限制
cmake_minimum_required(VERSION 3.20)
# 2. 项目名称
project(myApp)

message("当前项目路径: ${PROJECT_SOURCE_DIR}")

# 定义变量用于后续操作
#  1  设置静态库编译后的输出路径为./lib
set(LIB_PATH ${PROJECT_SOURCE_DIR}/lib)
#  2  设置可执行程序编译后的输出路径为./bin
set(EXEC_PATH ${PROJECT_SOURCE_DIR}/bin)
#  3  设置头文件的查找路径(要和你的实际路径保持一致!)
set(HEAD_PATH ${PROJECT_SOURCE_DIR}/include)
#  4  所调用的库的名字,也用变量保存
set(LABC_LIB labc)
set(LXYZ_LIB lxyz)
#  5  要构建的可执行程序的名字,也用变量保存
set(APP_NAME_1 myApp1)
set(APP_NAME_2 myApp2)

# 给当前节点增加子节点! 传入的是子文件夹的名字!
add_subdirectory(labc)
add_subdirectory(lxyz)
add_subdirectory(test1)
add_subdirectory(test2)

