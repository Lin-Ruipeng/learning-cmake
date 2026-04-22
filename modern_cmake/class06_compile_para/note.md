### 第三课：依赖管理 —— 零侵入式引入第三方库
前两课中，我们一直在假设环境中已经存在 `Eigen3::Eigen` 和 `SerialPort::SerialPort` 这两个 Target。但在真实的端侧部署场景中，你经常面临两个困境：
1.  **交叉编译环境下没有 `find_package`**：你在 x86 主机上为 ARM 板交叉编译 ONNX Runtime，系统 `/usr/lib` 里根本不存在 ARM 版本的 `.so`，`find_package(ONNXRuntime)` 必然失败。
2.  **"vendor" 目录"的野路子**：很多团队手动把第三方源码扔进项目的 `third_party/` 目录，然后用 `include_directories(${CMAKE_SOURCE_DIR}/third_party/...)` 手动拼接路径。这会导致版本管理混乱、路径硬编码、且无法利用现代 CMake 的传递性依赖特性。
现代 CMake 提供了两个优雅的解法，它们都能为你提供**干净的、带有完整编译属性（头文件、链接、编译选项）的 Import Target**（如 `Eigen3::Eigen`）。
#### 核心概念
**1. `find_package` —— 声明式发现**
*   **原理**：在系统中搜索预先安装好的库的 `.cmake` 配置文件（通常位于 `/usr/lib/cmake/PackageName/` 下），导入其导出的 Target。
*   **优势**：零构建开销，即插即用。
*   **局限**：依赖环境中必须已存在该库（宿主机编译、或通过包管理器预装）。
**2. `FetchContent` —— 配置时自动拉取（CMake 3.11+，3.20+ 完善）**
*   **原理**：在 CMake 的**配置阶段**（执行 `cmake ..` 时，而非编译阶段），自动从 Git 仓库或本地 URL 下载源码，并将其通过 `add_subdirectory` 的方式纳入当前构建树。
*   **优势**：实现构建环境的**完全自举**。无需用户手动安装任何依赖，`git clone` 之后直接 `cmake -B build` 即可编译。这对于包含 Header-Only 库（如 `Eigen`、`spdlog`、`nlohmann_json`）或需要特定源码版本的项目尤为强大。
*   **关键语法（CMake 3.20+ 推荐写法）**：
    ```cmake
    include(FetchContent)
    FetchContent_Declare(
      # 依赖名（自由命名，用于后续引用）
      my_lib_name
      # 数据源
      GIT_REPOSITORY https://github.com/xxx/yyy.git
      GIT_TAG        v1.2.3
    )
    # 可选：禁止该依赖被再次 populate（防止重复拉取）
    FetchContent_MakeAvailable(my_lib_name)
    ```
    执行后，该第三方库的 CMakeLists.txt 会被执行，它所创建的 Target 就在你的工程中可用了。
**3. 黄金法则：永远不要对 FetchContent 拉取的库手动拼路径**
只要一个第三方库自身使用的是现代 CMake（导出了 Target），你就**只需要** `target_link_libraries(your_target PRIVATE/ PUBLIC ThirdParty::Target)`。头文件路径、链接库路径全部由 Import Target 的 `INTERFACE` 属性自动传递。
---
### 实操出题：自包含的 IMU 工程
**场景描述**：
你的项目需要交付给另一个团队的同事，他不想手动安装 Eigen。你决定使用 `FetchContent` 让项目在配置阶段自动拉取 Eigen 源码，实现零配置构建。同时，你还需要使用 `find_package` 查找系统中已预装的 Threads 库（用于 `std::thread` 的底层 pthread 实现）。
**目录结构**：
```text
.
├── CMakeLists.txt              #  <-- 你只需要修改这一个文件
├── include/
│   └── fusion/
│       └── imu_fusion.h
├── src/
│   ├── driver/
│   │   ├── CMakeLists.txt      # 不需要修改
│   │   └── imu_driver.cpp
│   └── fusion/
│       ├── CMakeLists.txt      # 不需要修改
│       └── imu_fusion.cpp
```
**你的任务**：
**仅修改根目录的 `CMakeLists.txt`**，完成以下要求：
1.  使用 `FetchContent` 从 Git 仓库拉取 Eigen。Eigen 的官方仓库地址为 `https://gitlab.com/libeigen/eigen.git`，请使用标签 `3.4.0`。拉取完成后使其可用。
2.  使用 `find_package` 查找系统的 `Threads` 库（提示：`find_package(Threads REQUIRED)`，它会提供 `Threads::Threads` Target）。
3.  删除之前旧的手动 `find_package(Eigen3 REQUIRED)`。
4.  保持原有的 `add_subdirectory` 逻辑不变。
5.  假设 `driver` 模块内部使用了多线程（`std::thread`），请在根目录中通过一条 `target_link_libraries` 命令，将 `Threads::Threads` 以 `PRIVATE` 方式链接给 `driver`。
**关键提示**：
*   `FetchContent_Declare` 和 `FetchContent_MakeAvailable` 应该放在 `add_subdirectory` **之前**。因为 `fusion` 在构建时就需要 Eigen 已经被拉取好。
*   Eigen 是一个 Header-Only 库，它被 FetchContent 拉取后，会自动创建一个名为 `Eigen3::Eigen` 的 Target（和 `find_package` 产出的完全一致！），所以 `src/fusion/CMakeLists.txt` 里的 `target_link_libraries(fusion PUBLIC Eigen3::Eigen)` **一行都不需要改**。
请提交修改后的根目录 `CMakeLists.txt`，等待审查。
