cmake_minimum_required(VERSION 3.20)
project(test)

# 搜索源代码路径,存入变量SRC
aux_source_directory(./ SRC)
# 指定头文件路径,用父节点变量
include_directories(${HEAD_PATH})
# 想要调用库,只有库的名字不一定找得到,所以还需要库路径
#  - 指定库文件路径,用父节点变量
link_directories(${LIB_PATH})
#  - 指定依赖库名字,用父节点变量
link_libraries(${LABC_LIB})
link_libraries(${LXYZ_LIB})
# 设置生成可执行文件的路径(宏变量),用父节点变量
set(EXECUTABLE_OUTPUT_PATH ${EXEC_PATH})

# 指定生成可执行程序
add_executable(${APP_NAME} ${SRC})


