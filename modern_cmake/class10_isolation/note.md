你的嗅觉极其敏锐，堪称“代码审计员”级别的复盘！你完全抓到了盲点。
我必须向你坦白：**你是完全正确的，我在前九课的设计中，对这两个点做了“降维处理”。**
为什么这么做？因为前九课的核心目标是**“建立现代 CMake 的心智模型”**（Target、权限、导出）。如果在刚学走路的阶段，就强行塞入 `-O3 -march=native`（可能导致你本机的简单代码编译报错或行为异常），或者强行写 `toolchain.cmake`（要求你必须安装完整的 ARM 交叉编译工具链，否则连配置都过不去），会导致极高的试错挫败感。
但既然你已经顺利毕业，具备了扎实的基础，我们现在必须把这两块**真正在端侧部署中保命的高级技能**补上。
---
### 补充战役 1：第四阶段 —— HPC 细粒度优化（隔离算子编译 Flag）
在前面的第六课，你确实学了 `target_compile_options`，但当时加的是 `-Wall -Wextra`（**安全无害**的警告选项）。
而在真实的端侧 AI / 雷达信号处理中，我们要加的是**猛药**：`-O3`（激进循环展开）、`-march=native`（榨干当前 CPU 的指令集如 AVX2/NEON）、`-ffast-math`（牺牲一点精度换取极速浮点运算）。
**猛药如果全局喷洒，必死无疑。** 比如主控逻辑里的状态机加了 `-ffast-math`，会导致 `NaN != NaN` 的 IEEE 标准被破坏，引发诡异的死循环。
#### 实操出题：为 GeoUtils 注入“特效药”
**场景设定**：
在 `GeoUtils` 项目中，`geo_core` 只是普通的坐标计算，保持默认优化。但现在你要加一个新的静态库模块 `fast_matrix`，它负责密集的矩阵乘法，必须吃上“猛药”。
主程序 `app` **绝对不能**吃到这些猛药。
**你的任务**（在上一课的 GeoUtils 项目上修改）：
1.  **新建模块 `src/fast_matrix/`**：
    *   新建 `src/fast_matrix/fast_matrix.cpp`（里面随便写个空的函数即可）和 `CMakeLists.txt`。
2.  **核心挑战：编写 `src/fast_matrix/CMakeLists.txt`**：
    *   添加静态库 `fast_matrix`。
    *   使用 `target_compile_options(fast_matrix PRIVATE ...)` 注入以下三个选项：
        *   `-O3`
        *   `-march=native` （提示当前 CPU 架构可用的最高级指令集）
        *   `-ffast-math`
3.  **修改 `src/app/CMakeLists.txt`**：
    *   让 `app` 链接 `fast_matrix`（ PRIVATE ）。
4.  **验证手段**：
    *   因为你不能从运行结果看出优化区别，你需要通过**编译日志**来验证“隔离性”。
    *   运行构建命令时加上详细输出：`cmake --build build/debug -v`（或如果用的是 Makefiles，运行 `VERBOSE=1 cmake --build build/debug`）。
    *   **验收标准**：在满屏的编译日志中，你必须找到 `fast_matrix.cpp` 的编译命令，确认它以 `-O3 -march=native -ffast-math` 结尾；同时找到 `main.cpp` 的编译命令，确认它**没有**这三个参数（它应该只有默认的 `-g` 或 `-O0` 等 Debug 参数）。
请提交：**`src/fast_matrix/CMakeLists.txt`** 和 **修改后的 `src/app/CMakeLists.txt`**，并附带一段你从终端拷贝的**编译日志片段**（证明你看到了参数隔离）。
等待审查。通过后我们立刻进入最硬核的 `toolchain.cmake` 交叉编译！
