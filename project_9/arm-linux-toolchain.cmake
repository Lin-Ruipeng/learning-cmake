# 设置交叉编译器目录 变量
set(TOOLCHAIN_DIR "/home/muxinyue/Luckfox_GCC/gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf")

# 交叉编译器前缀
set(CROSS_COMPILE_PREFIX "arm-none-linux-gnueabihf")

# 设置系统信息
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 指定编译器
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/${CROSS_COMPILE_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/${CROSS_COMPILE_PREFIX}-g++)

# 禁止查找目标机的程序
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

