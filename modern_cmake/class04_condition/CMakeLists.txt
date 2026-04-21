# src/driver/CMakeLists.txt 构建 driver 静态库。要求它把 include/driver 作为 PUBLIC 路径暴露出去。

# 构建静态库
add_library(driver STATIC)
target_sources(driver PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imu_driver.cpp")
# target_include_directories(driver PUBLIC "../../include/driver")
target_include_directories(driver PUBLIC "${CMAKE_SOURCE_DIR}/include/driver")

# 最后一步, 添加库依赖(分平台条件编译)
if(WIN32)
  target_compile_definitions(driver PRIVATE USE_WIN_SERIAL)
elseif(UNIX)
  target_compile_definitions(driver PRIVATE USE_TERMIOS)
else() # 防呆设计, 避免出意外
  message(FATAL_ERROR "严重错误!无法确定编译平台! USE_WIN_SERIAL / USE_TERMIOS 宏均未被启用!")
endif()
