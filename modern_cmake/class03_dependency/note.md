### 第二课：工程骨架 —— 端侧项目的目录结构与模块化拆分
在第一课中，我们掌握了 Target 的权限控制。但在真实的大型端侧 AI 或导航项目中，把所有模块的 `add_library` 全部塞进根目录的 `CMakeLists.txt` 是不可接受的。这会导致根脚本臃肿不堪，违背了单一职责原则。
现代 C++ 工程的标准做法是**“源码与头文件物理分离”**加上**“构建脚本按目录分发”**。
#### 核心概念：目录作用域与子目录隔离
1.  **Out-of-Source 构建（绝对红线）**：
    永远不要在源码目录下执行 `cmake .`。必须创建一个独立的 `build/` 目录（如 `cmake -B build`）。这能保证源码树的绝对纯净，便于你用 Git 管理代码，也便于清理构建产物。
2.  **`add_subdirectory()` 的本质**：
    它不是简单的“包含文件”，而是**开启了一个新的 CMake 作用域**。在这个子目录里定义的变量、Target，默认是局限于这个子域的（除非使用 `PARENT_SCOPE` 显式抛出，但在现代 CMake 中极少这么做）。
3.  **Include 目录的规范演变**：
    *   `src/driver/imu_driver.h`（错：头文件和实现混在一起，暴露了内部结构）
    *   `include/driver/imu_driver.h` + `src/driver/imu_driver.cpp`（对：对外公开的头文件统一放在项目级的 `include/` 目录，外界引用时非常清晰）
#### 最佳实践范例（结构示意）
```text
# 推荐的工程拓扑
project_root/
├── CMakeLists.txt          # 顶层：只负责全局配置和 add_subdirectory 路由
├── include/                # 全局公共头文件池
│   └── driver/
│       └── imu_driver.h
├── src/
│   ├── driver/
│   │   ├── CMakeLists.txt  # 子模块：只负责构建 driver
│   │   └── imu_driver.cpp
│   └── fusion/
│       ├── CMakeLists.txt  # 子模块：只负责构建 fusion
│       └── imu_fusion.cpp
└── build/                  # (Out-of-Source) 编译发生在这里
```
---
### 实操出题：多目录模块化重构
**场景描述**：
项目规模扩大，我们需要将第一课的单体 `CMakeLists.txt` 拆分为符合上述标准拓扑的多目录结构。
**新的目录结构如下**：
```text
.
├── CMakeLists.txt
├── include/
│   ├── driver/
│   │   └── imu_driver.h    # 假设里面还是不暴露 SerialPort
│   └── fusion/
│       └── imu_fusion.h    # 假设里面依然有 #include <Eigen/Dense>
├── src/
│   ├── driver/
│   │   ├── CMakeLists.txt
│   │   └── imu_driver.cpp
│   └── fusion/
│       ├── CMakeLists.txt
│       └── imu_fusion.cpp
```
*(假设环境同第一课，已存在 `Eigen3::Eigen` 和 `SerialPort::SerialPort`)*
**你的任务**：
请分别写出以下**三个** CMakeLists.txt 的完整内容：
1.  **根目录的 `CMakeLists.txt`**：包含项目基本信息、`find_package`，并使用 `add_subdirectory` 将两个子模块引入（注意子目录的路径指向）。
2.  **`src/driver/CMakeLists.txt`**：构建 `driver` 静态库。要求它把 `include/driver` 作为 PUBLIC 路径暴露出去。
3.  **`src/fusion/CMakeLists.txt`**：构建 `fusion` 静态库。要求它把 `include/fusion` 作为 PUBLIC 路径暴露，并正确链接 Eigen 和 driver。
**关键挑战**：
在子目录的 `CMakeLists.txt` 中，`CMAKE_CURRENT_SOURCE_DIR` 指向的是 `src/driver/` 或 `src/fusion/`。你**如何精准地拼凑出上层 `include/` 目录的绝对路径**，来供给 `target_include_directories` 使用？（提示：利用 `CMAKE_CURRENT_SOURCE_DIR` 的相对路径向上跳跃 `../..`）。
请提交这三个文件的代码，等待审查。
