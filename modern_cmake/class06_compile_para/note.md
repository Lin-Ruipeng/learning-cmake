### 第六课：编译控制与特性开关 —— 拒绝全局毒气，实现精细化构建
在前五课中，我们已经把项目的架构、依赖、跨平台和安装导出做到了工业级水准。但还有一个极其重要的领域我们从未触碰：**编译器行为的控制**。
在真实的业务开发中，你经常面临这些需求：
*   “算法模块必须强制使用 C++17 标准，因为用到了 `std::optional`。”
*   “提交 CI 测试时，警告必须全开（`-Wall -Wextra`），把隐患扼杀在摇篮里。”
*   “给客户发版时，通过一个开关静默掉某些非核心的调试日志。”
很多开发者的本能反应是打开根目录的 `CMakeLists.txt` 狂洒全局指令：
```cmake
# 🚨 反模式：房间里的毒气
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra)
add_definitions(-DENABLE_DEBUG_LOG)
```
**为什么这些是毒气？**
1.  `set(CMAKE_CXX_STANDARD ...)` 是全局的，它**无法被导出**。当你把 `fusion` 库导出给别人时，别人 `find_package` 引入你的库，他的编译器并不知道你需要 C++17，导致别人的代码在编译时因为找不到 C++17 特性而报错。
2.  `add_compile_options` 会无差别强加给项目里的所有 Target，包括第三方的 `FetchContent` 拉下来的库（比如 Eigen）。给第三方库强加 `-Werror` 或额外警告，极易导致第三方库编译失败。
现代 CMake 的解法依然是：**精准投喂，Target 级控制**。
#### 核心概念
**1. 特性需求：`target_compile_features`**
不要用 `set(CMAKE_CXX_STANDARD 17)`，而要用：
```cmake
target_compile_features(fusion PUBLIC cxx_std_17)
```
*   **PUBLIC** 的妙用：这不仅让 `fusion` 以 C++17 编译，还会把“我需要 C++17”这个要求写进导出文件（`INTERFACE_COMPILE_FEATURES`）中。当别人 `target_link_libraries(their_app PRIVATE fusion)` 时，CMake 会**自动**强迫 `their_app` 也使用 C++17 编译！
**2. 选项开关：`option()`**
```cmake
option(ENABLE STRICT WARNINGS "Enable -Wall -Wextra for project targets" ON)
```
这会在 CMake GUI 或命令行（`-DENABLE_STRICT_WARNINGS=OFF`）中暴露一个布尔开关。然后通过 `if(ENABLE_STRICT_WARNINGS)` 配合 `target_compile_options` 进行精细化控制。
**3. 编译选项注入：`target_compile_options`**
```cmake
target_compile_options(driver PRIVATE -Wall -Wextra -Werror)
```
只影响 `driver`，绝不敢误伤 Eigen。
---
### 实操出题：为算法模块加上安全带与 C++17 特性
**场景描述**：
1. 你的 `fusion` 算法大量使用了 C++17 的结构化绑定和 `std::optional`，必须强制要求 C++17，并且这个要求必须能传递给最终的可执行文件。
2. 项目需要增加一个全局开关 `ENABLE_EXTRA_WARNINGS`。如果用户没有主动关闭它（默认 ON），则必须给 `driver` 和 `fusion` 加上 `-Wall -Wextra` 警告选项（仅限这两个内部 Target，严禁加给 Eigen）。
3. 为了跨平台兼容，警告选项不能直接写死 `-Wall`，因为 MSVC（Windows）不认这个。你需要用 `if(MSVC)` 做平台分支。
**你的任务**：
修改 `src/fusion/CMakeLists.txt` 和 根目录 `CMakeLists.txt`。
**具体要求**：
1.  **`src/fusion/CMakeLists.txt`**：
    *   使用 `target_compile_features` 为 `fusion` 添加 `cxx_std_17`，权限设为 `PUBLIC`。
2.  **根目录 `CMakeLists.txt`**：
    *   在 `project()` 之后，添加一个 `option(ENABLE_EXTRA_WARNINGS ...)`，默认值为 `ON`。
    *   在 `add_subdirectory` 之后，根据这个 option 的值，使用 `if()` 判断。
    *   在判断内部，使用 `if(MSVC)` 给出 MSVC 的警告选项（`/W4`），`else()` 给出 GCC/Clang 的选项（`-Wall -Wextra`）。
    *   使用 `target_compile_options` 分别将警告选项以 `PRIVATE` 方式添加给 `driver` 和 `fusion`。
请提交修改后的 **`src/fusion/CMakeLists.txt`** 和 **根目录 `CMakeLists.txt`**。等待审查。

---

先把上一课的点评说完，再专门解释你问的那条 CMP0146 警告。
---
## 一、第六课作业点评
### 整体评价：核心逻辑都对，只有两个小点要修
你这次的关键点都做到了：
- 用 `option(ENABLE_EXTRA_WARNINGS ...)` 把“是否开启额外警告”做成了用户可配置的开关，默认 ON——正确。
- 在根目录里用 `if(ENABLE_EXTRA_WARNINGS)` + `if(MSVC)` 对 driver 加 `/W4`，else 加 `-Wall -Wextra`——正确。
- 用 `target_compile_options(driver PRIVATE ...)` 限定给 driver，没有“毒气”到 Eigen——正确。
- 在 `src/fusion/CMakeLists.txt` 里用 `target_compile_features(fusion PUBLIC cxx_std_17)` 向上传播 C++17 要求——非常标准，这是第六课最核心的知识点，你已经掌握了【官方也推荐这种写法】。
下面是两个需要修正/补充的点。
---
## 二、需要修正的两点
### 1) “警告”只给了 driver，漏了 fusion
题目要求是：
- 给 **driver 和 fusion** 都加上额外警告；
- 不许给 Eigen 加。
你现在的实现只给了 driver：
```cmake
if(ENABLE_EXTRA_WARNINGS)
  if(MSVC)
    target_compile_options(driver PRIVATE /W4)
  else()
    target_compile_options(driver PRIVATE -Wall -Wextra)
  endif()
endif()
```
修正很简单：把相同逻辑对 `fusion` 再做一份：
```cmake
if(ENABLE_EXTRA_WARNINGS)
  if(MSVC)
    target_compile_options(driver PRIVATE /W4)
    target_compile_options(fusion PRIVATE /W4)
  else()
    target_compile_options(driver PRIVATE -Wall -Wextra)
    target_compile_options(fusion PRIVATE -Wall -Wextra)
  endif()
endif()
```
或者（如果你不想重复平台判断）：
```cmake
if(ENABLE_EXTRA_WARNINGS)
  if(MSVC)
    set(_warnings /W4)
  else()
    set(_warnings -Wall -Wextra)
  endif()
  target_compile_options(driver PRIVATE ${_warnings})
  target_compile_options(fusion PRIVATE ${_warnings})
endif()
```
> 注意：这里给 fusion 的警告用 `PRIVATE` 就够了，因为这只是我们对自己模块的“严格要求”，不是接口约束，没必要通过导出强加给别人。
---
### 2) 你的 message 是 WARNING，有点“吓人”
你写了：
```cmake
message(WARNING "ENABLE_EXTRA_WARNINGS is ON, compile options has -Wall -Wextra")
```
CMake 里 `message(WARNING ...)` 会以“黄字/警告条”的形式出现在配置日志里，很多工程师看到会以为是出错了。  
像“提示性信息”更适合用 `STATUS`，比如：
```cmake
message(STATUS "ENABLE_EXTRA_WARNINGS is ON, compile options has -Wall -Wextra")
```
这会在 CMake 配置输出里显示成普通信息，而不是警告。
---
## 三、关于你问的那条 CMP0146 警告：是什么？要管吗？
你看到的是：
```text
CMake Warning (dev) at build/_deps/eigen-src/unsupported/test/CMakeLists.txt:293 (find_package):
  Policy CMP0146 is not set: The FindCUDA module is removed.  Run "cmake
  --help-policy CMP0146" for policy details.  Use the cmake_policy command to
  set the policy and suppress this warning.
This warning is for project developers. Use -Wno-dev to suppress it.
```
拆开解释：
1) 这条警告“不属于你的项目”，属于 **Eigen 自己的测试 CMakeLists.txt**：
- 路径：`build/_deps/eigen-src/unsupported/test/CMakeLists.txt:293`——这是 `FetchContent` 拉下来的 Eigen 源码树里的测试脚本。
- 那里有个 `find_package(CUDA)` 调用，是 Eigen 用来做 CUDA 相关测试/示例的配置。
2) 为什么会触发警告？
- 你用的 CMake 版本 ≥ 3.27，而 CMake 3.27 把老的 `FindCUDA` 模块正式移除，并通过策略 CMP0146 来控制兼容行为：
  - 若没设置 CMP0146：CMake 会给出这条警告，但**仍然按老方式去加载被移除的 FindCUDA 模块（OLD 行为）**。
  - 若设置 `NEW`：`find_package(CUDA)` 会直接失败（相当于这个模块不存在）。
- Eigen 的这个测试文件里写了 `find_package(CUDA)`，所以在新版 CMake 下就会触发这条警告。
3) 对你的构建有没有影响？
- 没有。这只是 Eigen **测试目录**的警告，Eigen 核心库照样正常构建和使用。
- 即使你不用 CUDA，也不影响 IMU 工程本身。
4) 可以怎么“关掉/忽略”？
有三种常用方式（选其一即可）：
- A) 全局关掉所有开发者的警告（最简单，适合你在学习阶段）：
  ```cmake
  # 在根目录 CMakeLists.txt 的 cmake_minimum_required 下面加一行
  cmake_policy(SET CMP0146 NEW)
  ```
  这会让所有 `find_package(CUDA)` 都按“新行为”处理（相当于承认“不再使用老 FindCUDA 模块”），警告会消失。
- B) 只对 Eigen 的子目录关掉这条警告：
  ```cmake
  # 在 FetchContent_MakeAvailable(eigen) 之前：
  set(CMAKE_POLICY_DEFAULT_CMP0146 NEW)
  ```
- C) 不改任何东西，运行 CMake 时加参数忽略开发者的警告：
  ```bash
  cmake -B build -Wno-dev
  ```
  这会压制所有带 `(dev)` 标记的警告，包括这条。
**我的建议：**
- 如果只是做课程/练习，可以直接用 `cmake -B build -Wno-dev`，干净清爽。
- 如果是想“彻底正规”，在根目录的 `CMakeLists.txt` 里在 `project()` 之后加一行：
  ```cmake
  cmake_policy(SET CMP0146 NEW)
  ```
  这样以后再也不会被这条警告打扰。
---
## 四、审查结论
- 核心知识点（`option`、`target_compile_options`、`target_compile_features PUBLIC`）全部掌握；
- 需要补上对 `fusion` 的警告设置；
- 建议把 `message(WARNING ...)` 改为 `message(STATUS ...)`；
- CMP0146 警告来自 Eigen 的测试脚本，和你的工程无关，可忽略或用上述方式消除。
**修正以上两点后，第六课视为通过。** 直接继续的话可以回复：**继续下一课**。
