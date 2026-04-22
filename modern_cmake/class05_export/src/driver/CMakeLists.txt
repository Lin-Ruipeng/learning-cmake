# src/driver/CMakeLists.txt 构建 driver 静态库。要求它把 include/driver 作为 PUBLIC 路径暴露出去。

# 构建静态库
add_library(driver STATIC)
target_sources(driver PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imu_driver.cpp")
# 注意哦,需要区分构建时和作为库分发时对文件路径的写法
target_include_directories(
  driver PUBLIC "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/driver>"
                "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/driver>")
# 假设使用到了这个库就写: target_link_libraries(driver PRIVATE Threads::Threads)

# 导出与安装

# 1创建别名 (对外只暴露别名，隐藏内部真实的 target 名字)
add_library(ImuProject::driver ALIAS driver)

# 2安装头文件到系统的 include/driver/ 目录下 注意：这里直接用绝对路径找到源码树里的头文件，拷贝过去
install(FILES "${CMAKE_SOURCE_DIR}/include/driver/imu_driver.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/driver") # 头文件暴露

# 3安装Target并导出
install(
  TARGETS driver
  EXPORT ImuProjectTargets # 把这个 target 的属性扔进名为 ImuProjectTargets 的篮子里
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # 静态库
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # 动态库
)
