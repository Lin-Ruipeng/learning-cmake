# CMake Study — 从传统到现代的实战学习记录

> 一个从零开始学习 CMake 构建系统的工程化实践仓库，涵盖传统 CMake 基础操作与现代 CMake（3.12+）Target-Centric 范式的完整学习路径，最终场景收束于**端侧 AI 部署与 HPC 高性能计算**。

---

## 学习背景

本仓库记录了我作为 C++ 开发者的两轮 CMake 学习历程：

| 阶段 | 目录 | 学习方式 | 侧重点 |
|------|------|----------|--------|
| **第一阶段** | [`traditional_cmake/`](traditional_cmake/) | B 站视频教学 | CMake 基础语法、编译链接操作、交叉编译入门 |
| **第二阶段** | [`modern_cmake/`](modern_cmake/) | LLM 互动教学（SOP 严格流程） | 现代 CMake Target-Centric 范式、端侧 AI 与 HPC 工程化实战 |

第二阶段的 LLM 教学遵循严格的**状态机 SOP 工作流**（理论精讲 → 出题 → Code Review → 循环），提示词记录于 [`modern_cmake/prompt.md`](modern_cmake/prompt.md)，每课的核心概念与要求记录于各课 `note.md`。

---

## 仓库文件树

```
CMake_study/
├── README.md                          # 本文件
├── LICENSE                            # MIT License
├── .gitignore                         # Git 忽略规则 (build/, bin/, lib/, a.out)
├── .editorconfig                      # 编辑器统一配置
├── .clang-format                      # C++ 代码风格 (Google Style)
├── .cmake-format.yaml                 # CMake 脚本格式化规则
│
├── traditional_cmake/                 # 第一阶段：传统 CMake 学习
│   ├── project_1/                     #   简单的多文件编译
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   └── src/
│   ├── project_2/                     #   生成动态库
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   ├── src/
│   │   └── main.cpp
│   ├── project_3/                     #   生成静态库
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   ├── src/
│   │   └── main.cpp
│   ├── project_4/                     #   链接静态库 / 链接动态库
│   │   ├── CMakeLists.txt
│   │   ├── laCMakeLists.txt          #   静态库链接版本
│   │   ├── include/
│   │   ├── lib_a/                    #   预编译静态库
│   │   ├── lib_so/                   #   预编译动态库
│   │   └── main.cpp
│   ├── project_5/                     #   CMake 嵌套 — 多库编译
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   ├── labc/                     #   子库 labc
│   │   ├── lxyz/                     #   子库 lxyz
│   │   ├── test1/                    #   可执行程序 1
│   │   ├── test2/                    #   可执行程序 2
│   │   ├── lib/
│   │   └── bin/
│   ├── project_6/                     #   CMake 嵌套 — 静态库中链接静态库
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   ├── labc/
│   │   ├── lxyz/
│   │   ├── lchild/                   #   新增子库 lchild
│   │   ├── test/
│   │   ├── lib/
│   │   └── bin/
│   ├── project_7/                     #   CMake 嵌套 — 静态库中链接动态库
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   ├── labc/
│   │   ├── lxyz/
│   │   ├── lchild/
│   │   └── test/
│   ├── project_8/                     #   条件编译与构建类型 (Debug/Release)
│   │   ├── CMakeLists.txt
│   │   ├── CMakeLists-old.txt        #   参考旧版
│   │   ├── main.cpp
│   │   ├── cmd.md                    #   构建命令说明
│   │   └── bench_O3_VS_O0-g.md      #   性能基准测试数据
│   └── project_9/                     #   交叉编译 (ARM)
│       ├── CMakeLists.txt
│       ├── arm-linux-toolchain.cmake #   ARM 交叉编译工具链
│       ├── main.cpp
│       └── note.md                   #   交叉编译与部署流程
│
└── modern_cmake/                      # 第二阶段：现代 CMake 实战学习
    ├── prompt.md                      #   LLM 教学 SOP 提示词
    ├── class01_target/                #   第一课：Target 基础与权限传递
    │   ├── CMakeLists.txt
    │   ├── note.md
    │   ├── driver/
    │   ├── fusion/
    │   └── main.cpp
    ├── class02_struct/                #   第二课：目录结构与模块化拆分
    │   ├── CMakeLists.txt
    │   ├── note.md
    │   ├── include/
    │   └── src/
    ├── class03_dependency/            #   第三课：第三方依赖管理
    │   ├── CMakeLists.txt
    │   ├── note.md
    │   ├── include/
    │   └── src/
    ├── class04_condition/             #   第四课：跨平台宏定义注入
    │   ├── CMakeLists.txt
    │   ├── note.md
    │   └── imu_driver.cpp
    ├── class05_export/                #   第五课：安装与目标导出
    │   ├── CMakeLists.txt
    │   ├── note.md
    │   ├── cmake/
    │   ├── include/
    │   └── src/
    ├── class06_compile_para/          #   第六课：编译控制与特性开关
    │   ├── CMakeLists.txt
    │   ├── note.md
    │   ├── include/
    │   └── src/
    ├── class07_ctest/                 #   第七课：CTest 自动化测试
    │   ├── CMakeLists.txt
    │   ├── note.md
    │   ├── include/
    │   └── src/
    ├── class08_cmakejson/             #   第八课：CMakePresets 标准化
    │   ├── CMakeLists.txt
    │   ├── note.md
    │   ├── CMakePresets.json
    │   ├── main.cpp
    │   └── tests.cpp
    ├── class09_project_GeoUtils/      #   第九课：结业大作业 — 完整工程模板
    │   ├── README.md
    │   ├── CMakeLists.txt
    │   ├── CMakePresets.json
    │   ├── note.md
    │   ├── cmake/
    │   ├── include/
    │   └── src/
    ├── class10_isolation/             #   补充课 1：HPC 细粒度优化隔离
    │   ├── README.md
    │   ├── CMakeLists.txt
    │   ├── CMakePresets.json
    │   ├── note.md
    │   ├── cmake/
    │   ├── include/
    │   └── src/
    └── class11_cross_compile/         #   补充课 2：交叉编译与 Presets 集成
        ├── README.md
        ├── CMakeLists.txt
        ├── CMakePresets.json
        ├── note.md
        ├── cmake/
        ├── include/
        └── src/
```

---

## 第一阶段：传统 CMake 基础

> 目录：`traditional_cmake/` | 学习方式：B 站视频教学

每个 `project_N` 为一节独立课程，逐步深入 CMake 的核心构建操作。

| 课程 | 文件夹 | 掌握知识点 |
|:---:|--------|------------|
| 1 | `project_1` | **多文件编译基础** — `set()` / `aux_source_directory()` / `file(GLOB)` 源文件搜集；`include_directories()` 头文件搜索；`CMAKE_CXX_STANDARD` 设置 C++ 标准；`add_executable()` 生成可执行文件 |
| 2 | `project_2` | **生成动态库** — `add_library(xxx SHARED)` 构建共享库(.so)；理解动态库与可执行文件的构建差异 |
| 3 | `project_3` | **生成静态库** — `add_library(xxx STATIC)` 构建静态库(.a)；掌握静态库与动态库的构建指令区别 |
| 4 | `project_4` | **链接静态库 / 动态库** — `link_libraries()` 与 `link_directories()` 全局链接；`target_link_libraries()` 目标级链接；预编译库的引入方式 |
| 5 | `project_5` | **CMake 嵌套构建 — 多库编译** — `add_subdirectory()` 模块化拆分；父节点与子节点变量共享；`LIBRARY_OUTPUT_PATH` / `EXECUTABLE_OUTPUT_PATH` 宏变量控制输出；多个子目录库与可执行程序的协调管理 |
| 6 | `project_6` | **嵌套构建 — 静态库中链接静态库** — `add_definitions(-DDEBUG)` 全局宏定义；库间依赖链（labc → lxyz → lchild）；子模块逐级调用 |
| 7 | `project_7` | **嵌套构建 — 静态库中链接动态库** — `target_link_libraries()` 替代 `link_libraries()`；静态库内部链接动态库的配置技巧 |
| 8 | `project_8` | **条件编译与构建类型** — `CMAKE_BUILD_TYPE` 区分 Debug/Release 模式；`target_compile_options()` 精细化注入 `-g -O0` / `-O3`；`target_compile_features()` 指定 C++17；配合 `hyperfine` 进行性能基准测试 |
| 9 | `project_9` | **交叉编译** — 编写 ARM Toolchain 文件 (`arm-linux-toolchain.cmake`)；`CMAKE_C_COMPILER` / `CMAKE_CXX_COMPILER` 指定交叉编译器；`CMAKE_FIND_ROOT_PATH_MODE` 隔离宿主与目标机库搜索；`scp` 远程部署与执行 |

---

## 第二阶段：现代 CMake 实战

> 目录：`modern_cmake/` | 学习方式：LLM 互动教学（提示词 [`prompt.md`](modern_cmake/prompt.md)）

每个 `classXX_topic` 为一节课程，按端侧 AI 与 HPC 工程化场景逐层递进。教学采用严格的状态机 SOP：理论精讲 → 场景出题 → 代码提交 → 架构级 Code Review → 循环。

| 课程 | 文件夹 | 掌握知识点 |
|:---:|--------|------------|
| 1 | `class01_target` | **Target-Centric 范式转换** — `PUBLIC` / `PRIVATE` / `INTERFACE` 权限修饰符的依赖图论模型；`target_sources()` / `target_include_directories()` / `target_link_libraries()` / `target_compile_features()` 全套现代指令；`find_package(Eigen3)` 系统级第三方库发现；彻底摒弃全局污染指令 |
| 2 | `class02_struct` | **工程目录骨架与模块化拆分** — `add_subdirectory()` 实现源码与构建脚本按目录分发；`include/` 与 `src/` 物理分离的最佳实践；`CMAKE_CURRENT_SOURCE_DIR` vs `CMAKE_SOURCE_DIR` 的区别与路径拼接；Out-of-Source 构建 (绝对红线) |
| 3 | `class03_dependency` | **第三方依赖管理** — `include(FetchContent)` + `FetchContent_Declare()` + `FetchContent_MakeAvailable()` 三连从 Git 远程零侵入拉取 Eigen 3.4.0；`find_package(Threads REQUIRED)` 寻找系统线程库；依赖注入绝不手写路径拼接 |
| 4 | `class04_condition` | **跨平台宏定义注入** — `target_compile_definitions()` 以 PRIVATE 权限注入平台相关宏；`WIN32` / `UNIX` / `APPLE` 三路条件编译分支；`message(FATAL_ERROR ...)` 防呆设计；彻底告别 `add_definitions()` 全局毒气 |
| 5 | `class05_export` | **安装与目标导出** — `install(TARGETS ... EXPORT ...)` 架构级库导出；`GNUInstallDirs` 标准安装路径；`BUILD_INTERFACE` / `INSTALL_INTERFACE` 生成器表达式区分构建态与安装态头文件路径；`add_library(ALIAS)` 命名空间别名；`configure_file()` + `Config.cmake.in` 模板生成；`find_dependency()` 传递第三方依赖链 |
| 6 | `class06_compile_para` | **编译控制与特性开关** — `target_compile_features(PUBLIC cxx_std_17)` 细粒度 C++ 标准传递；`option()` 可配置编译开关；`target_compile_options(PRIVATE -Wall -Wextra)` / `MSVC /W4` 跨编译器警告注入；理解 CMP0146 策略警告及其正确处理 |
| 7 | `class07_ctest` | **CTest 自动化测试框架** — `enable_testing()` + `add_test()` 注册单元测试；`set_tests_properties(... TIMEOUT 30)` 超时管控；`add_subdirectory(tests)` 测试目录解耦；构建阶段与测试阶段严格分离 (`cmake --build` → `ctest`) |
| 8 | `class08_cmakejson` | **CMakePresets.json 构建流程标准化** — `version 6` 现代预设格式；`configurePresets` / `buildPresets` / `testPresets` 三件套；`"hidden": true` 抽象基类继承复用；`--preset` 一键切换 Debug/Release 全流程 |
| 9 | `class09_project_GeoUtils` | **结业大作业：完整工程模板** — 综合前三阶段全部技能：多模块库 (`geo_core` + `geo_algo`) + 可执行程序 (`app`) + 单元测试 (`test_core` / `test_algo`) + 跨平台宏 (GEO_WINDOWS / GEO_POSIX) + 安装导出 (namespace `GeoUtils::`) + CMakePresets 全链路；涵盖 `target_*` 全线现代指令 |
| 10 | `class10_isolation` | **HPC 细粒度优化 — 编译 Flag 按 Target 隔离** — 新增 `fast_matrix` 计算密集模块并注入 `-O3 -march=native -ffast-math`；严格使用 PRIVATE 权限确保优化 Flag 不污染其他 Target；通过 `VERBOSE=1 cmake --build` 编译日志验证隔离性 |
| 11 | `class11_cross_compile` | **交叉编译与 Presets 深度集成** — 编写 `aarch64_toolchain.cmake` 工具链文件；`CMAKE_SYSTEM_NAME` / `CMAKE_SYSTEM_PROCESSOR` 设定目标系统；`CMAKE_FIND_ROOT_PATH_MODE` 严格隔离宿主与目标机依赖搜索；通过 `CMakePresets.json` 的 `"toolchainFile"` 字段一键切换交叉编译配置 |

---

## 我的 LLM 教学 SOP

第二阶段采用严格的 SOP 工作流保证学习质量（完整提示词见 [`modern_cmake/prompt.md`](modern_cmake/prompt.md)）：

```
状态 0 (初始化) → 状态 1 (理论+出题) → 状态 2 (Code Review) → 状态 3 (循环)
```

- **状态 1**：LLM 输出核心概念与最佳实践范例，随后模拟 IMU/AI 模块场景布置实操题
- **状态 2**：对提交的 CMake 脚本执行红线审查（禁止全局污染指令）、作用域审查（PUBLIC/PRIVATE/INTERFACE）、部署审查（编译选项精准作用），不合格则要求修改并重审
- **严格红线**：严禁 `include_directories()` / `link_libraries()` / `add_definitions()` 等全局污染指令

---

## 使用方式

每个 project / class 目录均可独立构建运行。以现代 CMake 课程为例：

```bash
# 进入某一课程目录
cd modern_cmake/class09_project_GeoUtils

# 使用 Presets 一键配置与构建
cmake --preset debug
cmake --build --preset build-debug
ctest --preset test-debug

# 安装到指定前缀
cmake --install build/debug --prefix ./install
tree ./install
```

如果是不支持预设文件的项目：

```bash
cmake -B build
cmake --build build
```

交叉编译课程 (`class11_cross_compile`) 额外支持：

```bash
cmake --preset aarch64-linux    # 切换到 ARM 交叉编译配置
cmake --build build/aarch64-linux
```

---

## 工程配置

| 文件 | 用途 |
|------|------|
| `.editorconfig` | 统一 C++ / JSON / Python 文件的缩进风格（空格、宽度） |
| `.clang-format` | 基于 Google Style + 现代 C++ 微调（左指针对齐、100 列宽、include 自动排序） |
| `.cmake-format.yaml` | CMake 脚本格式化（100 列宽、4 空格缩进、命令全小写） |
| `.gitignore` | 排除 `build/`、`bin/`、`lib/`、`a.out` 等构建产物 |

---

## 许可

[MIT License](LICENSE) — Copyright (c) 2026 LinRuipeng
