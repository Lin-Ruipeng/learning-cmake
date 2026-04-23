# /CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(GeoUtils)

include(GNUInstallDirs) # GNU的安装目录

enable_testing() # 测试

# 条件编译
option(
    ENABLE_EXTRA_WARNINGS
    "打印额外的警告"
    ON)

add_subdirectory("${CMAKE_SOURCE_DIR}/src/geo_core")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/geo_algo")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/app")
add_subdirectory("${CMAKE_SOURCE_DIR}/src/tests")

# 安装和导出 1加上命名空间, 指定目标路径, 就是把所有的GeoUtilsTargets对象都放到指定目录
install(
    EXPORT GeoUtilsTargets
    NAMESPACE GeoUtils::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/GeoUtils")
# 复制配置文件
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/GeoUtilsConfig.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/GeoUtilsConfig.cmake")
# 再把生成的配置文件安装到 Targets 目录, 可以看到这里的第一个参数和上一行最后一个是一样的
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/GeoUtilsConfig.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/GeoUtils")
# 这里的目标路径和Targets的目标路径一致
