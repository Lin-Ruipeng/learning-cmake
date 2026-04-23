# GeoUtils/src/geo_algo/CMakeLists.txt
add_library(geo_algo STATIC)

target_sources(geo_algo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/collision.cpp")

target_include_directories(
    geo_algo PUBLIC "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/geo_algo>"
                    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/geo_algo>")

# 链接库, 且需要传递给下游
target_link_libraries(geo_algo PUBLIC geo_core)

# 条件编译参数
if(ENABLE_EXTRA_WARNINGS)
    message(STATUS "开启额外警告")
    if(WIN32)
        target_compile_options(geo_algo PRIVATE /W4)
    elseif(UNIX)
        target_compile_options(geo_algo PRIVATE -Wall -Wextra)
    else()
        message(FATAL_ERROR "不支持的平台!")
    endif()
endif()

# 安装导出 1起别名
add_library(
    GeoUtils::geo_algo
    ALIAS
    geo_algo)

# 2导出头文件
install(FILES "${CMAKE_SOURCE_DIR}/include/geo_algo/collision.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/geo_algo")

# 3导出源文件
install(
    TARGETS geo_algo
    EXPORT GeoUtilsTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INCLUDES
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
