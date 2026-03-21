cmake_minimum_required(VERSION 3.20)
project(lxyz)

# 搜索源文件到变量SRC
# 由于这个cmake文件和源码放在同一目录所以用./
aux_source_directory(./ SRC)
# 指定头文件路径,这里调用了父节点变量
include_directories(${HEAD_PATH})
# 设置生成的库文件输出位置,采用宏变量和父节点变量
set(LIBRARY_OUTPUT_PATH ${LIB_PATH})

# 注意,由于lxyz调用了child所以需要链接这个库!
link_libraries(${LCHILD_LIB})
# 链接库的同时,应该指明这个库所在的路径!
link_directories(${LIB_PATH})

# 这是个库,设置生成库的名字和源文件,名字用父节点变量
add_library(${LXYZ_LIB} STATIC ${SRC})

