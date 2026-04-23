# /src/fast_matrix/CMakeLists.txt
add_library(fast_matrix STATIC)
target_sources(fast_matrix PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/fast_matrix.cpp")
target_include_directories(
    fast_matrix PUBLIC "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/fast_matrix>"
                       "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/fast_matrix>")

# 特殊的编译需求, 私有化就不会影响到其他target
target_compile_options(
    fast_matrix
    PRIVATE -O3
            -march=native
            -ffast-math)

# 安装导出 1起别名
add_library(
    GeoUtils::fast_matrix
    ALIAS
    fast_matrix)
# 2安装头文件
install(FILES "${CMAKE_SOURCE_DIR}/include/fast_matrix/fast_matrix.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/fast_matrix")
# 3安装库文件
install(
    TARGETS fast_matrix
    EXPORT GeoUtilsTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INCLUDES
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
