### 第九课：结业大作业 —— 从零构建一个完整的现代 C++ 工程模板
恭喜你走到了这里。前八课你学会了每一块拼图，现在需要把它们**全部组装到一起**。
与之前每课给你详细指导不同，这一课我只会给你**需求规格书**和**目录骨架**。你需要独立完成全部文件的编写，就像在公司里接到第一个独立开发任务一样。
---
#### 项目名称：`GeoUtils`（几何工具库）
一个轻量级的 2D 几何计算库，提供点、矩形的基础操作和碰撞检测算法。
##### 目录结构
```
GeoUtils/
├── CMakeLists.txt               # 根目录（你来写）
├── CMakePresets.json            # 预设文件（你来写）
├── README.md                    # （不需要写）
├── include/
│   ├── geo_core/
│   │   └── point.h              # （骨架已给）
│   └── geo_algo/
│       └── collision.h          # （骨架已给）
├── src/
│   ├── geo_core/
│   │   ├── CMakeLists.txt       # （你来写）
│   │   └── point.cpp
│   ├── geo_algo/
│   │   ├── CMakeLists.txt       # （你来写）
│   │   └── collision.cpp
│   ├── app/
│   │   ├── CMakeLists.txt       # （你来写）
│   │   └── main.cpp
│   └── tests/
│       ├── CMakeLists.txt       # （你来写）
│       ├── test_core.cpp
│       └── test_algo.cpp
└── cmake/
    └── GeoUtilsConfig.cmake.in  # （你来写）
```
##### 需求规格书（逐条验收）
**【规则 R1】Target 拆分与依赖**
- `geo_core` 是静态库，暴露 `include/geo_core` 为 PUBLIC 路径，要求 C++17（PUBLIC 传播）。
- `geo_algo` 是静态库，暴露 `include/geo_algo` 为 PUBLIC 路径，PUBLIC 依赖 `geo_core`。
- `app/main` 是可执行文件，PRIVATE 依赖 `geo_algo`。
**【规则 R2】跨平台宏定义】**
- `geo_core` 在 Windows 下需要宏 `GEO_WINDOWS`，Linux 下需要 `GEO_POSIX`。通过 `if(WIN32)/elseif(UNIX)` 实现。
- 跨平台宏定义只影响库自身编译，不需要传播给下游（用 `PRIVATE`）。
**【规则 R3】安装与导出】**
- 两个库都需要创建别名（`GeoUtils::geo_core`、`GeoUtils::geo_algo`），并导出。
- 头文件安装到 `include/geo_core/` 和 `include/geo_algo/`。
- 库文件安装到标准 `lib/` 目录。
- `app/main` **不许**导出。
- 根目录需要 `include(GNUInstallDirs)`。
- 导出文件安装到 `${CMAKE_INSTALL_LIBDIR}/cmake/GeoUtils/`。
- 需要编写 `cmake/GeoUtilsConfig.cmake.in`。
**【规则 R4】测试】**
- `test_core` 链接 `geo_core`，`test_algo` 链接 `geo_algo`。
- 两个测试都注册到 CTest，超时 10 秒。
- 使用 `$<TARGET_FILE:...>` 注册测试命令。
**【规则 R5】构建预设】**
- 编写 `CMakePresets.json`，包含：
  - 隐藏 `base`（Unix Makefiles，自动输出目录，开启 compile_commands）。
  - `debug` 预设和 `release` 预设。
  - 对应的 `build` 预设和 `test` 预设。
**【规则 R6】额外警告开关】**
- 根目录定义 `option(ENABLE_EXTRA_WARNINGS "..." ON)`。
- 开启时给 `geo_core` 和 `geo_algo` 加上 `-Wall -Wextra`（GCC/Clang）或 `/W4`（MSVC）。
---
##### 头文件骨架（直接复制使用）
**`include/geo_core/point.h`**：
```cpp
#ifndef GEO_CORE_POINT_H
#define GEO_CORE_POINT_H
struct Point {
    double x;
    double y;
};
double distance(const Point& a, const Point& b);
#endif
```
**`include/geo_algo/collision.h`**：
```cpp
#ifndef GEO_ALGO_COLLISION_H
#define GEO_ALGO_COLLISION_H
#include "point.h"
struct Rect {
    Point min;
    Point max;
};
bool rect_intersect(const Rect& a, const Rect& b);
#endif
```
##### 源文件骨架（直接复制使用）
**`src/geo_core/point.cpp`**：
```cpp
#include "point.h"
#include <cmath>
double distance(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}
```
**`src/geo_algo/collision.cpp`**：
```cpp
#include "collision.h"
bool rect_intersect(const Rect& a, const Rect& b) {
    if (a.max.x < b.min.x || b.max.x < a.min.x) return false;
    if (a.max.y < b.min.y || b.max.y < a.min.y) return false;
    return true;
}
```
**`src/app/main.cpp`**：
```cpp
#include "collision.h"
#include "point.h"
#include <iostream>
int main() {
    Point p1{0.0, 0.0};
    Point p2{3.0, 4.0};
    std::cout << "Distance: " << distance(p1, p2) << std::endl;
    Rect a{{0, 0}, {2, 2}};
    Rect b{{1, 1}, {3, 3}};
    std::cout << "Intersect: " << (rect_intersect(a, b) ? "true" : "false") << std::endl;
    return 0;
}
```
**`src/tests/test_core.cpp`**：
```cpp
#include "point.h"
#include <iostream>
int main() {
    Point p1{0.0, 0.0};
    Point p2{3.0, 4.0};
    double d = distance(p1, p2);
    if (d > 4.99 && d < 5.01) {
        std::cout << "[PASS] distance test" << std::endl;
        return 0;
    }
    std::cout << "[FAIL] distance test, got " << d << std::endl;
    return 1;
}
```
**`src/tests/test_algo.cpp`**：
```cpp
#include "collision.h"
#include <iostream>
int main() {
    Rect a{{0, 0}, {2, 2}};
    Rect b{{1, 1}, {3, 3}};
    Rect c{{10, 10}, {12, 12}};
    if (rect_intersect(a, b) && !rect_intersect(a, c)) {
        std::cout << "[PASS] collision test" << std::endl;
        return 0;
    }
    std::cout << "[FAIL] collision test" << std::endl;
    return 1;
}
```
---
##### 验收标准
你需要提交 **7 个文件**的内容：
| 序号 | 文件 | 对应规则 |
|------|------|----------|
| 1 | `CMakeLists.txt`（根目录） | R1, R2, R3, R4, R6 |
| 2 | `src/geo_core/CMakeLists.txt` | R1, R2, R3 |
| 3 | `src/geo_algo/CMakeLists.txt` | R1, R3 |
| 4 | `src/app/CMakeLists.txt` | R1 |
| 5 | `src/tests/CMakeLists.txt` | R4 |
| 6 | `cmake/GeoUtilsConfig.cmake.in` | R3 |
| 7 | `CMakePresets.json` | R5 |
并且你需要保证本地能通过：
```bash
cmake --preset build-debug
ctest --preset test-debug
cmake --install build/debug --prefix ./install
tree ./install
```
这是一次真正的**独立作战**。之前的每节课你都已经学过了对应的知识点，现在把它们拼起来。
请提交你的 7 个文件，等待最终审查。
