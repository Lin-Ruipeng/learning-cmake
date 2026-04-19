### 第一课：范式重塑 —— Target 基础与权限传递
在传统 CMake 中，`include_directories()` 就像是在房间里释放毒气——只要在这个目录下的所有 C++ 文件，无论愿不愿意，都会呼吸到这个头文件路径。这在大型工程中会导致严重的符号冲突和编译加速失效。
现代 CMake 的核心哲学是**“面向目标编程”**。一个 Target（如通过 `add_library` 或 `add_executable` 创建）是一个自治的实体，它封装了自己的源码、头文件路径、编译选项和链接依赖。
#### 核心概念：PPI 权限修饰符的图论逻辑
当我们使用 `target_*` 系列命令时，必须指定一个修饰符。你可以把 Target 之间的依赖看作一张有向图（A -> B 表示 A 链接了 B）：
1.  **`PRIVATE`（私有，阻断传递）**：
    *   **作用**：仅对当前 Target 生效，**不会**传递给任何链接当前 Target 的下游 Target。
    *   **场景**：内部实现细节。比如 A 库内部用了某个第三方库来做日志打印，但 A 暴露给外部的头文件里完全没有提到这个日志库。下游 B 链接 A 时，根本不需要知道日志库的存在。
2.  **`INTERFACE`（接口，纯传递）**：
    *   **作用**：对当前 Target **不生效**，但会**传递**给下游 Target。
    *   **场景**：Header-Only 库。因为它没有 `.cpp` 文件需要编译，自己不需要任何包含路径，但它必须把这些路径传递给使用它的人。
3.  **`PUBLIC`（公开，自己用并传递）**：
    *   **作用**：当前 Target 生效，**且传递**给下游 Target。
    *   **场景**：你在 A 的**公开头文件**（即被 `#include` 到外部的头文件）里使用了某个类型。下游 B 包含了 A 的头文件，编译器必然需要那个类型的定义，所以这个依赖必须传递下去。
#### 最佳实践范例（纯粹的 Target 风格）
```cmake
# 假设这是一个数学计算库
add_library(MathCore STATIC math.cpp)
# 这个库内部用了 <thread>，需要 C++17，但对外头文件没暴露多线程类型
target_compile_features(MathCore PRIVATE cxx_std_17)
# 假设这是一个滤波器库，依赖 MathCore
add_library(ImuFilter STATIC filter.cpp)
# 滤波器的头文件里大量使用了 MathCore 的矩阵类型 -> 必须 PUBLIC
target_link_libraries(ImuFilter PUBLIC MathCore)
# 滤波器内部实现用到了一个第三方插值库，但头文件没暴露 -> 必须 PRIVATE
target_link_libraries(ImuFilter PRIVATE InterpolationLib)
```
---
### 实操出题：IMU 预处理与解算模块拆分
**场景描述**：
你正在编写一个 IMU 导航模块。工程中有两个核心组件：
1.  `ImuDriver`：负责底层硬件数据读取与预处理（如去除零偏）。它内部使用了一个第三方的串口库 `SerialPort` 来读取数据，但它的对外头文件 `imu_driver.h` 里完全没有提到串口相关的东西。
2.  `ImuFusion`：负责姿态解算。它依赖 `ImuDriver` 提供的干净数据。并且，在 `ImuFusion` 的**公开头文件** `imu_fusion.h` 中，直接使用了第三方矩阵库 `Eigen` 的 `Eigen::Vector3d` 类型。
**当前项目目录结构**：
```text
.
├── CMakeLists.txt
├── driver/
│   ├── imu_driver.cpp
│   └── imu_driver.h      # 内部只有纯数据结构，无 Eigen，无 SerialPort
├── fusion/
│   ├── imu_fusion.cpp
│   └── imu_fusion.h      # 包含了 #include <Eigen/Dense>
```
**假设环境**：
系统中已经通过 `find_package` 找到了 `Eigen3`（提供 `Eigen3::Eigen` Target）和 `SerialPort`（提供 `SerialPort::SerialPort` Target）。你不需要写 `find_package`，直接当做可用的 Target 使用即可。
**你的任务**：
请在根目录的 `CMakeLists.txt` 中，使用现代 CMake 语法写出这两个静态库的构建逻辑。
**要求**：
1. 严禁出现任何全局指令（如 `include_directories`, `link_libraries` 等）。
2. 精确、严格地使用 `PUBLIC`、`PRIVATE` 为这两个 Target 配置依赖。
3. 请提交你的完整 CMakeLists.txt 代码。

# 执行
```bash
rm -rf build          # 清理旧的构建缓存
cmake -B build        # 重新配置
cmake --build build   # 编译
./build/main          # 运行
```
