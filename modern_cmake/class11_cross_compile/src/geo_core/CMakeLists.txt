# GeoUtils/src/geo_core/CMakeLists.txt
add_library(geo_core STATIC)

target_sources(geo_core PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/point.cpp")

# 注意build和install的区别
target_include_directories(
    geo_core PUBLIC "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/geo_core>"
                    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/geo_core>")

# 公开传递编译需求
target_compile_features(geo_core PUBLIC cxx_std_17)

# 跨平台适配, 不同平台定义不同的宏, 注入到源码中
if(WIN32)
    message(STATUS "为win32平台编译")
    target_compile_definitions(geo_core PRIVATE GEO_WINDOWS)
elseif(UNIX)
    message(STATUS "为unix平台编译")
    target_compile_definitions(geo_core PRIVATE GEO_POSIX)
else()
    message(FATAL_ERROR "不支持的平台!")
endif()

# 条件编译参数
if(ENABLE_EXTRA_WARNINGS)
    message(STATUS "开启额外警告")
    if(WIN32)
        target_compile_options(geo_core PRIVATE /W4)
    elseif(UNIX)
        target_compile_options(geo_core PRIVATE -Wall -Wextra)
    else()
        message(FATAL_ERROR "不支持的平台!")
    endif()
endif()

# 1先要创建别名在安装
add_library(
    GeoUtils::geo_core
    ALIAS
    geo_core)

# 2头文件的安装, 指定文件到目标目录
install(FILES "${CMAKE_SOURCE_DIR}/include/geo_core/point.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/geo_core")

# 3源文件的安装, 指定目标复制到指定目录, 并导出到指定的汇总里
install(
    TARGETS geo_core
    EXPORT GeoUtilsTargets # 把当前target导出到汇总的targets里
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INCLUDES
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
