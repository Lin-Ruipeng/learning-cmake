### 第七课：质量守护 —— 引入 CTest 自动化测试框架
在前六课中，我们解决了“如何优雅地构建和分发代码”的问题。但一个成熟的工业级项目，还必须解决“如何证明代码没被改坏”的问题。
很多初学者的做法是：写一个 `test_main.cpp`，编译成可执行文件，然后手动去 `./test_main` 看输出。这在本地可以，但在 CI/CD（持续集成）流水线中完全行不通。流水线需要一个统一的、能自动收集失败状态、支持超时控制、并且能输出标准测试报告的工具。
这就是 CTest 存在的意义。它是 CMake 原生集成的测试驱动器。
#### 核心概念：分离“编译”与“运行”
很多人学 CTest 时会踩坑，原因是没有搞清 CMake 在这里的角色分工：
1.  **构建阶段**：你需要像平时一样，用 `add_executable` 把测试代码编译成 `.exe` 或可执行文件。
2.  **注册阶段**：使用 `add_test(NAME <名字> COMMAND <可执行文件>)`。**注意：这行代码不编译任何东西！** 它只是向 CMake 的一个内部名单里登记：“等会儿跑测试的时候，请帮我执行这个程序”。
3.  **运行阶段**：在终端输入 `ctest`（不带 `--build` 参数），CTest 会按名单逐个执行程序，检查它们的返回值（返回 0 代表 PASS，非 0 代表 FAIL）。
#### 核心命令
*   `enable_testing()`：在根目录开启测试功能（这相当于给 CMake 引擎通电）。
*   `add_test(NAME ... COMMAND ...)`：注册一个测试用例。
*   `set_tests_properties(... PROPERTIES TIMEOUT ...)`：给测试加上超时秒数，防止死循环卡死整个 CI 流水线。
---
### 实操出题：为 Fusion 算法添加单元测试
**场景描述**：
你需要为 `fusion` 模块编写一个基础的单测。要求单测代码和被测模块放在一起（这叫“就近测试”原则，是 Google、Meta 等大厂的标配），但单测生成的可执行文件**绝对不能**被安装到系统中。
**你的任务**：
新建文件并修改现有 CMake 脚本。
**1. 新建测试源文件：`src/fusion/tests/test_fusion.cpp`**
随便写一点伪测试逻辑（不需要引入真正的测试框架如 GTest，用简单的 `return 0;` 模拟通过即可）：
```cpp
#include <iostream>
#include "imu_fusion.h" // 引用被测模块的头文件
int main() {
    std::cout << "[TEST] Running fusion basic test..." << std::endl;
    // 这里假装做了一些断言
    std::cout << "[TEST] Fusion test passed!" << std::endl;
    return 0; // 返回 0 代表测试通过
}
```
**2. 新建测试构建脚本：`src/fusion/tests/CMakeLists.txt`**
*   使用 `add_executable` 构建名为 `test_fusion` 的可执行文件。
*   使用 `target_link_libraries` 将 `fusion` 链接给它（注意权限，测试消费库，应该用 `PRIVATE`）。
*   使用 `add_test` 注册这个可执行文件。
*   使用 `set_tests_properties` 给这个测试设置 5 秒超时。
**3. 修改 `src/fusion/CMakeLists.txt`**
*   在文件末尾加上 `add_subdirectory(tests)`，把测试目录纳入构建树。
**4. 修改根目录 `CMakeLists.txt`**
*   在 `project(imu_project)` 紧接着的下方，加上 `enable_testing()`。
**关键提醒**：
*   **不要**在测试的 CMakeLists 里写 `install(TARGETS test_fusion ...)`。测试代码是内部机密，绝不交付给客户。
*   `add_test(COMMAND ...)` 后面跟的**必须是你 `add_executable` 定义的名字**，不能跟 `.cpp` 源文件名。
请提交这四个文件的内容（新建的两个文件 + 修改的两个 `CMakeLists.txt`），等待审查。

# 需要在构建之后手动运行测试
```bash
# 1. 配置（生成构建系统）
cmake -B build

# 2. 编译（构建项目）
cmake --build build

# 3. 运行测试（执行所有已注册的测试用例）
cd build          # ctest 需要在构建目录下运行
ctest             # 或者直接 ctest --test-dir build (CMake 3.20+)
```
