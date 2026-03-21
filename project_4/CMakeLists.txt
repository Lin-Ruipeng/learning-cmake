# 链接动态库
cmake_minimum_required(VERSION 3.20)

project(myapp)

# 搜索源文件
aux_source_directory(./ SRC)

# 头文件路径
include_directories(./include)

## 链接静态库
## link_libraries(abc)

# 库路径
# link_directories(./lib_a)
# 动态库路径
link_directories(./lib_so)

add_executable(myApp ${SRC})

message("尝试构建动态库版本")

# 链接动态库
target_link_libraries(myApp abc)

