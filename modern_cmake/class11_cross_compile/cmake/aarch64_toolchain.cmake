# 工具链文件: cmake/aarch64_toolchain.cmake

# 1指定目标系统
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# 2强制指定编译器 (注意必须是 COMPILER 结尾！！！)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# 3寻址策略隔离 (端侧部署核心防御机制) ONLY: 告诉 find_package 和 find_library，只能在 ARM 的 sysroot 里找库，绝对不能去你 Ubuntu 宿主机的
# /usr/lib 里找！
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
