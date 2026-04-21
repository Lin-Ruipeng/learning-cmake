### 第四课：平台感知 —— 跨平台宏定义的 Target 级注入
在前三课中，我们构建的工程一直在你当前的操作系统上平滑运行。但在真实的端侧交付中，你的 IMU 算法可能今天要在 Ubuntu 上跑单元测试，明天就要交叉编译到 RTOS 或 Windows 工控机上。
不同平台的底层 API 天差地别（比如 Linux 的串口是 `<termios.h>`，Windows 是 `<windows.h>`）。C++ 代码中通常用预处理器宏 `#ifdef` 来做条件编译，而 CMake 的职责是**准确地告诉编译器，当前正在为哪个平台构建，应该定义哪些宏**。
#### 核心概念：告别全局宏毒气
**反模式（绝对禁止）**：
```cmake
# 这是在房间里释放毒气的另一种形式
add_definitions(-DUSE_TERMIOS)
add_definitions(-DUSE_WIN_SERIAL)
```
一旦使用 `add_definitions`，这些宏会无差别地强加给当前目录及其所有子目录下的所有 C++ 文件，哪怕是一个完全不涉及底层的纯数学头文件，也会被迫看到这些脏宏，极易引发隐蔽的宏冲突。
**现代模式**：
使用 `target_compile_definitions`，它和 `target_link_libraries` 一样，严格遵循 PPI（PUBLIC/PRIVATE/INTERFACE）权限传递图。
#### 平台判断的惯用语法
现代 CMake 提供了极其简洁的平台判断变量（注意不需要加 `${}` 进行解引用，直接作为布尔值判断）：
*   `if(WIN32)`：Windows 平台（无论是 32 位还是 64 位）。
*   `if(UNIX)`：类 Unix 系统（包括 Linux、macOS、Android 等）。
*   `if(APPLE)`：苹果系统（macOS、iOS 等）。
**最佳实践范例**：
```cmake
# 假设这是一个网络库，内部需要区分平台 API
add_library(MyNet STATIC net.cpp)
if(WIN32)
    # 注意：CMake 会自动加上 -D 前缀，你只需要写宏的名字
    target_compile_definitions(MyNet PRIVATE USE_WIN_SOCKET)
elseif(UNIX)
    target_compile_definitions(MyNet PRIVATE USE_POSIX_SOCKET)
endif()
```
---
### 实操出题：IMU 底层驱动的跨平台适配
**场景描述**：
你的 `driver` 模块需要进行跨平台重构。底层实现人员要求：
*   如果在 **Linux/macOS** 下编译，必须向编译器传入宏 `USE_TERMIOS=1`。
*   如果在 **Windows** 下编译，必须向编译器传入宏 `USE_WIN_SERIAL=1`。
*   这些宏属于底层硬件交互的**绝对机密**，绝不能泄露到上层的 `fusion` 算法模块中。
**目录结构**：
```text
.
├── CMakeLists.txt          # 不需要修改
├── src/
│   ├── driver/
│   │   ├── CMakeLists.txt  # <-- 你只需要修改这一个文件
│   │   └── imu_driver.cpp
│   └── fusion/
│       └── ...
```
**你的任务**：
修改 `src/driver/CMakeLists.txt`。
**要求**：
1.  严禁使用 `add_definitions`。
2.  使用 `if-elseif-endif` 结合 `target_compile_definitions` 为 `driver` 目标添加上述平台宏。
3.  严格控制权限，确保这些宏定义的可见性仅限于 `driver` 内部（使用正确的 PPI 修饰符）。
4.  保留之前课程中已经写好的 `add_library`、`target_sources` 和 `target_include_directories` 等逻辑。
请提交修改后的 `src/driver/CMakeLists.txt`，等待审查。
