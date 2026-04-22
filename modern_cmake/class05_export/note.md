### 第五课：构建闭环 —— 现代安装与目标导出
在前四课中，你掌握了如何优雅地构建一个复杂的 C++ 项目。但到目前为止，我们的成果都被“锁”在了 `build/` 目录里，只能自嗨。
在真实的工业界，当你写好了一个优秀的 IMU 融合算法库 `fusion`，你需要将其**打包交付**给另一个团队（比如做无人机控制的同学）。那个团队绝不想去研究你的源码目录结构，他们只想要一句话引入：
```cmake
# 另一个团队的 CMakeLists.txt
find_package(ImuProject REQUIRED)
target_link_libraries(their_drone PRIVATE ImuProject::fusion)
```
要实现这种像 Eigen 一样丝滑的体验，你必须掌握现代 CMake 的最后一块拼图：**安装与导出**。
#### 核心概念：Target 属性的序列化
还记得我们前几课辛辛苦苦配置的 `target_include_directories(... PUBLIC ...)` 和 `target_link_libraries(... PUBLIC ...)` 吗？这些属性目前只存在于 CMake 的内存中。
**导出的本质，就是把这些 Target 的编译属性（头文件路径、链接依赖、编译选项）序列化写入到 `.cmake` 文本文件中。**
1.  **`install(TARGETS ... EXPORT <ExportName>)`**
    将编译好的 `.a`/`.so` 文件拷贝到安装目录（如 `lib/`），并将该 Target 的属性收集到名为 `<ExportName>` 的集合中。
2.  **`install(EXPORT <ExportName> ...)`**
    将刚才收集的属性集合，生成一个 `ImuProjectTargets.cmake` 文件，放到安装目录的 `lib/cmake/ImuProject/` 下。
3.  **`Aliased Target (别名目标)` —— 工业界铁律**
    绝不能直接把内部 Target 名字（如 `fusion`）暴露给外部！外部用户如果执行了 `target_compile_definitions(fusion PRIVATE ...)`，会直接篡改你内部的构建逻辑。必须在内部创建一个只读的别名：`add_library(ImuProject::fusion ALIAS fusion)`。只把别名导出。
4.  **`include(GNUInstallDirs)`**
    跨平台的标准路径变量。`CMAKE_INSTALL_LIBDIR` 在 Linux 下是 `lib`，在 Windows 下是 `bin`。用它代替硬编码路径。
---
### 实操出题：让 ImuProject 成为系统级依赖
**场景描述**：
你需要改造现有的工程，使其可以通过 `cmake --install .` 安装到系统中，并允许外部 `find_package` 找到它。
**你的任务**：
需要修改三个文件，并新建一个配置文件。
**1. 新建配置文件模板：`cmake/ImuProjectConfig.cmake.in`**
（注意后缀是 `.in`，CMake 会替换里面的变量）
```cmake
# 这是一个模板文件，CMake 会在 install 时把 @EXPORT_NAME@ 替换为实际的文件名
include("${CMAKE_CURRENT_LIST_DIR}/ImuProjectTargets.cmake")
```
**2. 修改 `src/driver/CMakeLists.txt`**
在现有的 `target_include_directories` 之后，添加：
*   创建别名 `ImuProject::driver`。
*   安装头文件：将 `include/driver/` 下的头文件安装到系统的 `include/driver/` 目录（提示：使用 `install(FILES ... DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/driver)`）。
*   安装目标并导出：`install(TARGETS driver EXPORT ImuProjectTargets ...)`，同时指定 `ARCHIVE DESTINATION` (静态库后缀) 和 `LIBRARY DESTINATION` (动态库后缀) 为 `${CMAKE_INSTALL_LIBDIR}`。
**3. 修改 `src/fusion/CMakeLists.txt`**
类似 driver：
*   创建别名 `ImuProject::fusion`。
*   安装头文件到 `${CMAKE_INSTALL_INCLUDEDIR}/fusion`。
*   安装目标并导出：`install(TARGETS fusion EXPORT ImuProjectTargets ...)`。**注意：因为 fusion 依赖 Eigen3::Eigen 和 driver，在安装导出时，必须加上 `EXPORT_LINK_INTERFACE_LIBRARIES` 相关处理。但实际上现代 CMake 会自动处理依赖链，你只需像平时一样正常写出 ARCHIVE/LIBRARY DESTINATION 即可。**
**4. 修改根目录 `CMakeLists.txt`**
在文件末尾添加安装全局导出信息的逻辑：
*   引入 `include(GNUInstallDirs)`。
*   导出 Target 集合：`install(EXPORT ImuProjectTargets NAMESPACE ImuProject:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ImuProject)`（`NAMESPACE` 极其关键，它会在生成的文件里给所有 Target 自动加上 `ImuProject::` 前缀）。
*   安装 Config 文件：使用 `configure_file` 将刚才写的 `cmake/ImuProjectConfig.cmake.in` 生成到 build 目录，然后用 `install(FILES ...)` 将生成的 Config 文件安装到 `${CMAKE_INSTALL_LIBDIR}/cmake/ImuProject`。
**关键提示**：
*   `EXPORT ImuProjectTargets` 这个字符串必须在子目录和根目录中**完全保持一致**。
*   不要尝试导出 `main` 可执行文件，只导出库。
*   暂时不要管 `SerialPort::SerialPort` 和 `Threads::Threads` 的导出问题（假设它们会被系统自动找到）。
请提交修改后的**四个文件**的内容（`src/driver/CMakeLists.txt`、`src/fusion/CMakeLists.txt`、根目录 `CMakeLists.txt`、`cmake/ImuProjectConfig.cmake.in`），等待审查。

---

完全可以理解。安装与导出是现代 CMake 中**最抽象、最难调试**的部分，因为它涉及“跨工程”的状态传递。
我不让你凭空猜了。我直接给你提供一个**可以直接复制运行的完整标准模板**，并在每一行加上保姆级注释。你现在的任务是**读懂**它，理解每个词的作用，然后作为标准答案提交。
### 核心魔法揭秘：`INCLUDES DESTINATION`
在老版本的 CMake 中，导出头文件路径需要写极其恶心的 `INTERFACE_INCLUDE_DIRECTORIES`。但在 CMake 3.15 之后，有一个**极大地简化生活的神级属性**：`INCLUDES DESTINATION`。
只要在 `install(TARGETS ...)` 时加上这一句，CMake 会**自动**把你之前用 `target_include_directories(... PUBLIC ...)` 设置的路径，写入到导出文件中！不需要你手动拼接！
---
### 完整标准答案（请直接提交以下四个文件内容）
#### 1. 配置文件模板：`cmake/ImuProjectConfig.cmake.in`
（这个文件的作用极其单一：当别人执行 `find_package(ImuProject)` 时，CMake 会自动找到这个文件并执行它。它唯一要做的就是加载导出的 Target 列表。）
```cmake
# 当别人 find_package(ImuProject) 时，会执行这个文件
# 这里的 ImuProjectTargets.cmake 就是 CMake 自动帮我们生成的包含所有头文件、链接关系的文件
include("${CMAKE_CURRENT_LIST_DIR}/ImuProjectTargets.cmake")
```
#### 2. 修改 `src/driver/CMakeLists.txt`
（核心变化：加别名、安装头文件、安装 Target 并带上神级属性）
```cmake
# 构建静态库 (保持不变)
add_library(driver STATIC)
target_sources(driver PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imu_driver.cpp")
target_include_directories(driver PUBLIC "${CMAKE_SOURCE_DIR}/include/driver")
# 平台宏定义 (保持你上一课的完美答案)
if(WIN32)
  target_compile_definitions(driver PRIVATE USE_WIN_SERIAL)
elseif(UNIX)
  target_compile_definitions(driver PRIVATE USE_TERMIOS)
else()
  message(FATAL_ERROR "严重错误!无法确定编译平台!")
endif()
# ======== 以下为第五课新增：导出与安装 ========
# 1. 创建别名 (对外只暴露别名，隐藏内部真实的 target 名字)
add_library(ImuProject::driver ALIAS driver)
# 2. 安装头文件到系统的 include/driver/ 目录下
# 注意：这里直接用绝对路径找到源码树里的头文件，拷贝过去
install(FILES "${CMAKE_SOURCE_DIR}/include/driver/imu_driver.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/driver)
# 3. 安装 Target 并导出
install(TARGETS driver
        EXPORT ImuProjectTargets     # 把这个 target 的属性扔进名为 ImuProjectTargets 的篮子里
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  # 静态库 放到 lib/
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}  # 动态库 放到 lib/ (如果是 .so)
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # 神级属性：自动把 PUBLIC 头文件路径写进导出文件！
)
```
#### 3. 修改 `src/fusion/CMakeLists.txt`
（逻辑与 driver 完全对称）
```cmake
# 构建静态库 (保持不变)
add_library(fusion STATIC)
target_sources(fusion PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imu_fusion.cpp")
target_include_directories(fusion PUBLIC "${CMAKE_SOURCE_DIR}/include/fusion")
target_link_libraries(fusion PUBLIC Eigen3::Eigen PRIVATE driver)
# ======== 以下为第五课新增：导出与安装 ========
# 1. 创建别名
add_library(ImuProject::fusion ALIAS fusion)
# 2. 安装头文件
install(FILES "${CMAKE_SOURCE_DIR}/include/fusion/imu_fusion.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fusion)
# 3. 安装 Target 并导出
# 重点：虽然 fusion 依赖 Eigen 和 driver，但在现代 CMake 中，这些依赖关系会自动穿透写入导出文件！
install(TARGETS fusion
        EXPORT ImuProjectTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
```
#### 4. 修改根目录 `CMakeLists.txt`
（收尾工作：把篮子里的东西打包成文件，并安装 Config 文件）
```cmake
cmake_minimum_required(VERSION 3.20)
project(imu_project)
# 引入标准的安装路径变量 (CMAKE_INSTALL_LIBDIR, CMAKE_INSTALL_INCLUDEDIR 等)
include(GNUInstallDirs)
# 依赖拉取与模块引入 (保持不变)
include(FetchContent)
FetchContent_Declare(eigen GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git GIT_TAG 3.4.0)
FetchContent_MakeAvailable(eigen)
find_package(Threads REQUIRED)
add_subdirectory("src/driver")
add_subdirectory("src/fusion")
# 可执行文件不需要导出给外部，正常编译即可
add_executable(main "src/main.cpp")
target_link_libraries(main PRIVATE fusion)
# ======== 以下为第五课新增：全局导出配置 ========
# 1. 生成并安装 Export 文件
# 把子目录里收集到的 ImuProjectTargets 篮子，写入到 ImuProjectTargets.cmake 文件中
# NAMESPACE 极其关键：它会让文件里的 target 自动加上 "ImuProject::" 前缀！
install(EXPORT ImuProjectTargets
        NAMESPACE ImuProject::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ImuProject)
# 2. 处理 Config 模板文件并安装
# configure_file 会把 .in 文件复制一份到 build 目录 (替换掉里面的变量，这里没变量所以原样复制)
configure_file(cmake/ImuProjectConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/ImuProjectConfig.cmake
               COPYONLY)
# 把生成的 Config 文件安装到和 Targets 文件同一个目录下
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ImuProjectConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ImuProject)
```
---
### 验证方法（学有余力可以试一下）
提交通过后，你可以在终端执行以下命令，亲眼见证奇迹：
```bash
# 1. 正常编译
cmake -B build
cmake --build build
# 2. 执行安装（默认安装到 /usr/local，你可以改成安装到当前目录的 install 文件夹以防污染系统）
cmake --install build --prefix ./install
# 3. 观察生成的产物
tree ./install
# 你会看到：
# install/
# ├── include/driver/imu_driver.h
# ├── include/fusion/imu_fusion.h
# └── lib/cmake/ImuProject/
#     ├── ImuProjectConfig.cmake      <-- 别人 find_package 找的就是它
#     └── ImuProjectTargets.cmake     <-- 里面记录了所有的头文件和 Eigen 依赖
```
请将这四个文件的内容作为你的回复提交。

```bash
➜  class05_export git:(main) ✗ tree ./install
./install
├── include
│   ├── driver
│   │   └── imu_driver.h
│   ├── eigen3
│   │   ├── Eigen
│   │   │   ├── Cholesky
│   │   │   ├── CholmodSupport
│   │   │   ├── Core
│   │   │   ├── Dense
│   │   │   ├── Eigen
│   │   │   ├── Eigenvalues
│   │   │   ├── Geometry
│   │   │   ├── Householder
│   │   │   ├── IterativeLinearSolvers
│   │   │   ├── Jacobi
│   │   │   ├── KLUSupport
│   │   │   ├── LU
│   │   │   ├── MetisSupport
│   │   │   ├── OrderingMethods
│   │   │   ├── PaStiXSupport
│   │   │   ├── PardisoSupport
│   │   │   ├── QR
│   │   │   ├── QtAlignedMalloc
│   │   │   ├── SPQRSupport
│   │   │   ├── SVD
│   │   │   ├── Sparse
│   │   │   ├── SparseCholesky
│   │   │   ├── SparseCore
│   │   │   ├── SparseLU
│   │   │   ├── SparseQR
│   │   │   ├── StdDeque
│   │   │   ├── StdList
│   │   │   ├── StdVector
│   │   │   ├── SuperLUSupport
│   │   │   ├── UmfPackSupport
│   │   │   └── src
│   │   │       ├── Cholesky
│   │   │       │   ├── LDLT.h
│   │   │       │   ├── LLT.h
│   │   │       │   └── LLT_LAPACKE.h
│   │   │       ├── CholmodSupport
│   │   │       │   └── CholmodSupport.h
│   │   │       ├── Core
│   │   │       │   ├── ArithmeticSequence.h
│   │   │       │   ├── Array.h
│   │   │       │   ├── ArrayBase.h
│   │   │       │   ├── ArrayWrapper.h
│   │   │       │   ├── Assign.h
│   │   │       │   ├── AssignEvaluator.h
│   │   │       │   ├── Assign_MKL.h
│   │   │       │   ├── BandMatrix.h
│   │   │       │   ├── Block.h
│   │   │       │   ├── BooleanRedux.h
│   │   │       │   ├── CommaInitializer.h
│   │   │       │   ├── ConditionEstimator.h
│   │   │       │   ├── CoreEvaluators.h
│   │   │       │   ├── CoreIterators.h
│   │   │       │   ├── CwiseBinaryOp.h
│   │   │       │   ├── CwiseNullaryOp.h
│   │   │       │   ├── CwiseTernaryOp.h
│   │   │       │   ├── CwiseUnaryOp.h
│   │   │       │   ├── CwiseUnaryView.h
│   │   │       │   ├── DenseBase.h
│   │   │       │   ├── DenseCoeffsBase.h
│   │   │       │   ├── DenseStorage.h
│   │   │       │   ├── Diagonal.h
│   │   │       │   ├── DiagonalMatrix.h
│   │   │       │   ├── DiagonalProduct.h
│   │   │       │   ├── Dot.h
│   │   │       │   ├── EigenBase.h
│   │   │       │   ├── ForceAlignedAccess.h
│   │   │       │   ├── Fuzzy.h
│   │   │       │   ├── GeneralProduct.h
│   │   │       │   ├── GenericPacketMath.h
│   │   │       │   ├── GlobalFunctions.h
│   │   │       │   ├── IO.h
│   │   │       │   ├── IndexedView.h
│   │   │       │   ├── Inverse.h
│   │   │       │   ├── Map.h
│   │   │       │   ├── MapBase.h
│   │   │       │   ├── MathFunctions.h
│   │   │       │   ├── MathFunctionsImpl.h
│   │   │       │   ├── Matrix.h
│   │   │       │   ├── MatrixBase.h
│   │   │       │   ├── NestByValue.h
│   │   │       │   ├── NoAlias.h
│   │   │       │   ├── NumTraits.h
│   │   │       │   ├── PartialReduxEvaluator.h
│   │   │       │   ├── PermutationMatrix.h
│   │   │       │   ├── PlainObjectBase.h
│   │   │       │   ├── Product.h
│   │   │       │   ├── ProductEvaluators.h
│   │   │       │   ├── Random.h
│   │   │       │   ├── Redux.h
│   │   │       │   ├── Ref.h
│   │   │       │   ├── Replicate.h
│   │   │       │   ├── Reshaped.h
│   │   │       │   ├── ReturnByValue.h
│   │   │       │   ├── Reverse.h
│   │   │       │   ├── Select.h
│   │   │       │   ├── SelfAdjointView.h
│   │   │       │   ├── SelfCwiseBinaryOp.h
│   │   │       │   ├── Solve.h
│   │   │       │   ├── SolveTriangular.h
│   │   │       │   ├── SolverBase.h
│   │   │       │   ├── StableNorm.h
│   │   │       │   ├── StlIterators.h
│   │   │       │   ├── Stride.h
│   │   │       │   ├── Swap.h
│   │   │       │   ├── Transpose.h
│   │   │       │   ├── Transpositions.h
│   │   │       │   ├── TriangularMatrix.h
│   │   │       │   ├── VectorBlock.h
│   │   │       │   ├── VectorwiseOp.h
│   │   │       │   ├── Visitor.h
│   │   │       │   ├── arch
│   │   │       │   │   ├── AVX
│   │   │       │   │   │   ├── Complex.h
│   │   │       │   │   │   ├── MathFunctions.h
│   │   │       │   │   │   ├── PacketMath.h
│   │   │       │   │   │   └── TypeCasting.h
│   │   │       │   │   ├── AVX512
│   │   │       │   │   │   ├── Complex.h
│   │   │       │   │   │   ├── MathFunctions.h
│   │   │       │   │   │   ├── PacketMath.h
│   │   │       │   │   │   └── TypeCasting.h
│   │   │       │   │   ├── AltiVec
│   │   │       │   │   │   ├── Complex.h
│   │   │       │   │   │   ├── MathFunctions.h
│   │   │       │   │   │   ├── MatrixProduct.h
│   │   │       │   │   │   ├── MatrixProductCommon.h
│   │   │       │   │   │   ├── MatrixProductMMA.h
│   │   │       │   │   │   └── PacketMath.h
│   │   │       │   │   ├── CUDA
│   │   │       │   │   │   └── Complex.h
│   │   │       │   │   ├── Default
│   │   │       │   │   │   ├── BFloat16.h
│   │   │       │   │   │   ├── ConjHelper.h
│   │   │       │   │   │   ├── GenericPacketMathFunctions.h
│   │   │       │   │   │   ├── GenericPacketMathFunctionsFwd.h
│   │   │       │   │   │   ├── Half.h
│   │   │       │   │   │   ├── Settings.h
│   │   │       │   │   │   └── TypeCasting.h
│   │   │       │   │   ├── GPU
│   │   │       │   │   │   ├── MathFunctions.h
│   │   │       │   │   │   ├── PacketMath.h
│   │   │       │   │   │   └── TypeCasting.h
│   │   │       │   │   ├── HIP
│   │   │       │   │   │   └── hcc
│   │   │       │   │   │       └── math_constants.h
│   │   │       │   │   ├── MSA
│   │   │       │   │   │   ├── Complex.h
│   │   │       │   │   │   ├── MathFunctions.h
│   │   │       │   │   │   └── PacketMath.h
│   │   │       │   │   ├── NEON
│   │   │       │   │   │   ├── Complex.h
│   │   │       │   │   │   ├── GeneralBlockPanelKernel.h
│   │   │       │   │   │   ├── MathFunctions.h
│   │   │       │   │   │   ├── PacketMath.h
│   │   │       │   │   │   └── TypeCasting.h
│   │   │       │   │   ├── SSE
│   │   │       │   │   │   ├── Complex.h
│   │   │       │   │   │   ├── MathFunctions.h
│   │   │       │   │   │   ├── PacketMath.h
│   │   │       │   │   │   └── TypeCasting.h
│   │   │       │   │   ├── SVE
│   │   │       │   │   │   ├── MathFunctions.h
│   │   │       │   │   │   ├── PacketMath.h
│   │   │       │   │   │   └── TypeCasting.h
│   │   │       │   │   ├── SYCL
│   │   │       │   │   │   ├── InteropHeaders.h
│   │   │       │   │   │   ├── MathFunctions.h
│   │   │       │   │   │   ├── PacketMath.h
│   │   │       │   │   │   ├── SyclMemoryModel.h
│   │   │       │   │   │   └── TypeCasting.h
│   │   │       │   │   └── ZVector
│   │   │       │   │       ├── Complex.h
│   │   │       │   │       ├── MathFunctions.h
│   │   │       │   │       └── PacketMath.h
│   │   │       │   ├── functors
│   │   │       │   │   ├── AssignmentFunctors.h
│   │   │       │   │   ├── BinaryFunctors.h
│   │   │       │   │   ├── NullaryFunctors.h
│   │   │       │   │   ├── StlFunctors.h
│   │   │       │   │   ├── TernaryFunctors.h
│   │   │       │   │   └── UnaryFunctors.h
│   │   │       │   ├── products
│   │   │       │   │   ├── GeneralBlockPanelKernel.h
│   │   │       │   │   ├── GeneralMatrixMatrix.h
│   │   │       │   │   ├── GeneralMatrixMatrixTriangular.h
│   │   │       │   │   ├── GeneralMatrixMatrixTriangular_BLAS.h
│   │   │       │   │   ├── GeneralMatrixMatrix_BLAS.h
│   │   │       │   │   ├── GeneralMatrixVector.h
│   │   │       │   │   ├── GeneralMatrixVector_BLAS.h
│   │   │       │   │   ├── Parallelizer.h
│   │   │       │   │   ├── SelfadjointMatrixMatrix.h
│   │   │       │   │   ├── SelfadjointMatrixMatrix_BLAS.h
│   │   │       │   │   ├── SelfadjointMatrixVector.h
│   │   │       │   │   ├── SelfadjointMatrixVector_BLAS.h
│   │   │       │   │   ├── SelfadjointProduct.h
│   │   │       │   │   ├── SelfadjointRank2Update.h
│   │   │       │   │   ├── TriangularMatrixMatrix.h
│   │   │       │   │   ├── TriangularMatrixMatrix_BLAS.h
│   │   │       │   │   ├── TriangularMatrixVector.h
│   │   │       │   │   ├── TriangularMatrixVector_BLAS.h
│   │   │       │   │   ├── TriangularSolverMatrix.h
│   │   │       │   │   ├── TriangularSolverMatrix_BLAS.h
│   │   │       │   │   └── TriangularSolverVector.h
│   │   │       │   └── util
│   │   │       │       ├── BlasUtil.h
│   │   │       │       ├── ConfigureVectorization.h
│   │   │       │       ├── Constants.h
│   │   │       │       ├── DisableStupidWarnings.h
│   │   │       │       ├── ForwardDeclarations.h
│   │   │       │       ├── IndexedViewHelper.h
│   │   │       │       ├── IntegralConstant.h
│   │   │       │       ├── MKL_support.h
│   │   │       │       ├── Macros.h
│   │   │       │       ├── Memory.h
│   │   │       │       ├── Meta.h
│   │   │       │       ├── NonMPL2.h
│   │   │       │       ├── ReenableStupidWarnings.h
│   │   │       │       ├── ReshapedHelper.h
│   │   │       │       ├── StaticAssert.h
│   │   │       │       ├── SymbolicIndex.h
│   │   │       │       └── XprHelper.h
│   │   │       ├── Eigenvalues
│   │   │       │   ├── ComplexEigenSolver.h
│   │   │       │   ├── ComplexSchur.h
│   │   │       │   ├── ComplexSchur_LAPACKE.h
│   │   │       │   ├── EigenSolver.h
│   │   │       │   ├── GeneralizedEigenSolver.h
│   │   │       │   ├── GeneralizedSelfAdjointEigenSolver.h
│   │   │       │   ├── HessenbergDecomposition.h
│   │   │       │   ├── MatrixBaseEigenvalues.h
│   │   │       │   ├── RealQZ.h
│   │   │       │   ├── RealSchur.h
│   │   │       │   ├── RealSchur_LAPACKE.h
│   │   │       │   ├── SelfAdjointEigenSolver.h
│   │   │       │   ├── SelfAdjointEigenSolver_LAPACKE.h
│   │   │       │   └── Tridiagonalization.h
│   │   │       ├── Geometry
│   │   │       │   ├── AlignedBox.h
│   │   │       │   ├── AngleAxis.h
│   │   │       │   ├── EulerAngles.h
│   │   │       │   ├── Homogeneous.h
│   │   │       │   ├── Hyperplane.h
│   │   │       │   ├── OrthoMethods.h
│   │   │       │   ├── ParametrizedLine.h
│   │   │       │   ├── Quaternion.h
│   │   │       │   ├── Rotation2D.h
│   │   │       │   ├── RotationBase.h
│   │   │       │   ├── Scaling.h
│   │   │       │   ├── Transform.h
│   │   │       │   ├── Translation.h
│   │   │       │   ├── Umeyama.h
│   │   │       │   └── arch
│   │   │       │       └── Geometry_SIMD.h
│   │   │       ├── Householder
│   │   │       │   ├── BlockHouseholder.h
│   │   │       │   ├── Householder.h
│   │   │       │   └── HouseholderSequence.h
│   │   │       ├── IterativeLinearSolvers
│   │   │       │   ├── BasicPreconditioners.h
│   │   │       │   ├── BiCGSTAB.h
│   │   │       │   ├── ConjugateGradient.h
│   │   │       │   ├── IncompleteCholesky.h
│   │   │       │   ├── IncompleteLUT.h
│   │   │       │   ├── IterativeSolverBase.h
│   │   │       │   ├── LeastSquareConjugateGradient.h
│   │   │       │   └── SolveWithGuess.h
│   │   │       ├── Jacobi
│   │   │       │   └── Jacobi.h
│   │   │       ├── KLUSupport
│   │   │       │   └── KLUSupport.h
│   │   │       ├── LU
│   │   │       │   ├── Determinant.h
│   │   │       │   ├── FullPivLU.h
│   │   │       │   ├── InverseImpl.h
│   │   │       │   ├── PartialPivLU.h
│   │   │       │   ├── PartialPivLU_LAPACKE.h
│   │   │       │   └── arch
│   │   │       │       └── InverseSize4.h
│   │   │       ├── MetisSupport
│   │   │       │   └── MetisSupport.h
│   │   │       ├── OrderingMethods
│   │   │       │   ├── Amd.h
│   │   │       │   ├── Eigen_Colamd.h
│   │   │       │   └── Ordering.h
│   │   │       ├── PaStiXSupport
│   │   │       │   └── PaStiXSupport.h
│   │   │       ├── PardisoSupport
│   │   │       │   └── PardisoSupport.h
│   │   │       ├── QR
│   │   │       │   ├── ColPivHouseholderQR.h
│   │   │       │   ├── ColPivHouseholderQR_LAPACKE.h
│   │   │       │   ├── CompleteOrthogonalDecomposition.h
│   │   │       │   ├── FullPivHouseholderQR.h
│   │   │       │   ├── HouseholderQR.h
│   │   │       │   └── HouseholderQR_LAPACKE.h
│   │   │       ├── SPQRSupport
│   │   │       │   └── SuiteSparseQRSupport.h
│   │   │       ├── SVD
│   │   │       │   ├── BDCSVD.h
│   │   │       │   ├── JacobiSVD.h
│   │   │       │   ├── JacobiSVD_LAPACKE.h
│   │   │       │   ├── SVDBase.h
│   │   │       │   └── UpperBidiagonalization.h
│   │   │       ├── SparseCholesky
│   │   │       │   ├── SimplicialCholesky.h
│   │   │       │   └── SimplicialCholesky_impl.h
│   │   │       ├── SparseCore
│   │   │       │   ├── AmbiVector.h
│   │   │       │   ├── CompressedStorage.h
│   │   │       │   ├── ConservativeSparseSparseProduct.h
│   │   │       │   ├── MappedSparseMatrix.h
│   │   │       │   ├── SparseAssign.h
│   │   │       │   ├── SparseBlock.h
│   │   │       │   ├── SparseColEtree.h
│   │   │       │   ├── SparseCompressedBase.h
│   │   │       │   ├── SparseCwiseBinaryOp.h
│   │   │       │   ├── SparseCwiseUnaryOp.h
│   │   │       │   ├── SparseDenseProduct.h
│   │   │       │   ├── SparseDiagonalProduct.h
│   │   │       │   ├── SparseDot.h
│   │   │       │   ├── SparseFuzzy.h
│   │   │       │   ├── SparseMap.h
│   │   │       │   ├── SparseMatrix.h
│   │   │       │   ├── SparseMatrixBase.h
│   │   │       │   ├── SparsePermutation.h
│   │   │       │   ├── SparseProduct.h
│   │   │       │   ├── SparseRedux.h
│   │   │       │   ├── SparseRef.h
│   │   │       │   ├── SparseSelfAdjointView.h
│   │   │       │   ├── SparseSolverBase.h
│   │   │       │   ├── SparseSparseProductWithPruning.h
│   │   │       │   ├── SparseTranspose.h
│   │   │       │   ├── SparseTriangularView.h
│   │   │       │   ├── SparseUtil.h
│   │   │       │   ├── SparseVector.h
│   │   │       │   ├── SparseView.h
│   │   │       │   └── TriangularSolver.h
│   │   │       ├── SparseLU
│   │   │       │   ├── SparseLU.h
│   │   │       │   ├── SparseLUImpl.h
│   │   │       │   ├── SparseLU_Memory.h
│   │   │       │   ├── SparseLU_Structs.h
│   │   │       │   ├── SparseLU_SupernodalMatrix.h
│   │   │       │   ├── SparseLU_Utils.h
│   │   │       │   ├── SparseLU_column_bmod.h
│   │   │       │   ├── SparseLU_column_dfs.h
│   │   │       │   ├── SparseLU_copy_to_ucol.h
│   │   │       │   ├── SparseLU_gemm_kernel.h
│   │   │       │   ├── SparseLU_heap_relax_snode.h
│   │   │       │   ├── SparseLU_kernel_bmod.h
│   │   │       │   ├── SparseLU_panel_bmod.h
│   │   │       │   ├── SparseLU_panel_dfs.h
│   │   │       │   ├── SparseLU_pivotL.h
│   │   │       │   ├── SparseLU_pruneL.h
│   │   │       │   └── SparseLU_relax_snode.h
│   │   │       ├── SparseQR
│   │   │       │   └── SparseQR.h
│   │   │       ├── StlSupport
│   │   │       │   ├── StdDeque.h
│   │   │       │   ├── StdList.h
│   │   │       │   ├── StdVector.h
│   │   │       │   └── details.h
│   │   │       ├── SuperLUSupport
│   │   │       │   └── SuperLUSupport.h
│   │   │       ├── UmfPackSupport
│   │   │       │   └── UmfPackSupport.h
│   │   │       ├── misc
│   │   │       │   ├── Image.h
│   │   │       │   ├── Kernel.h
│   │   │       │   ├── RealSvd2x2.h
│   │   │       │   ├── blas.h
│   │   │       │   ├── lapack.h
│   │   │       │   ├── lapacke.h
│   │   │       │   └── lapacke_mangling.h
│   │   │       └── plugins
│   │   │           ├── ArrayCwiseBinaryOps.h
│   │   │           ├── ArrayCwiseUnaryOps.h
│   │   │           ├── BlockMethods.h
│   │   │           ├── CommonCwiseBinaryOps.h
│   │   │           ├── CommonCwiseUnaryOps.h
│   │   │           ├── IndexedViewMethods.h
│   │   │           ├── MatrixCwiseBinaryOps.h
│   │   │           ├── MatrixCwiseUnaryOps.h
│   │   │           └── ReshapedMethods.h
│   │   ├── signature_of_eigen3_matrix_library
│   │   └── unsupported
│   │       └── Eigen
│   │           ├── AdolcForward
│   │           ├── AlignedVector3
│   │           ├── ArpackSupport
│   │           ├── AutoDiff
│   │           ├── BVH
│   │           ├── CXX11
│   │           │   ├── Tensor
│   │           │   ├── TensorSymmetry
│   │           │   ├── ThreadPool
│   │           │   └── src
│   │           │       ├── Tensor
│   │           │       │   ├── Tensor.h
│   │           │       │   ├── TensorArgMax.h
│   │           │       │   ├── TensorAssign.h
│   │           │       │   ├── TensorBase.h
│   │           │       │   ├── TensorBlock.h
│   │           │       │   ├── TensorBroadcasting.h
│   │           │       │   ├── TensorChipping.h
│   │           │       │   ├── TensorConcatenation.h
│   │           │       │   ├── TensorContraction.h
│   │           │       │   ├── TensorContractionBlocking.h
│   │           │       │   ├── TensorContractionCuda.h
│   │           │       │   ├── TensorContractionGpu.h
│   │           │       │   ├── TensorContractionMapper.h
│   │           │       │   ├── TensorContractionSycl.h
│   │           │       │   ├── TensorContractionThreadPool.h
│   │           │       │   ├── TensorConversion.h
│   │           │       │   ├── TensorConvolution.h
│   │           │       │   ├── TensorConvolutionSycl.h
│   │           │       │   ├── TensorCostModel.h
│   │           │       │   ├── TensorCustomOp.h
│   │           │       │   ├── TensorDevice.h
│   │           │       │   ├── TensorDeviceCuda.h
│   │           │       │   ├── TensorDeviceDefault.h
│   │           │       │   ├── TensorDeviceGpu.h
│   │           │       │   ├── TensorDeviceSycl.h
│   │           │       │   ├── TensorDeviceThreadPool.h
│   │           │       │   ├── TensorDimensionList.h
│   │           │       │   ├── TensorDimensions.h
│   │           │       │   ├── TensorEvalTo.h
│   │           │       │   ├── TensorEvaluator.h
│   │           │       │   ├── TensorExecutor.h
│   │           │       │   ├── TensorExpr.h
│   │           │       │   ├── TensorFFT.h
│   │           │       │   ├── TensorFixedSize.h
│   │           │       │   ├── TensorForcedEval.h
│   │           │       │   ├── TensorForwardDeclarations.h
│   │           │       │   ├── TensorFunctors.h
│   │           │       │   ├── TensorGenerator.h
│   │           │       │   ├── TensorGlobalFunctions.h
│   │           │       │   ├── TensorGpuHipCudaDefines.h
│   │           │       │   ├── TensorGpuHipCudaUndefines.h
│   │           │       │   ├── TensorIO.h
│   │           │       │   ├── TensorImagePatch.h
│   │           │       │   ├── TensorIndexList.h
│   │           │       │   ├── TensorInflation.h
│   │           │       │   ├── TensorInitializer.h
│   │           │       │   ├── TensorIntDiv.h
│   │           │       │   ├── TensorLayoutSwap.h
│   │           │       │   ├── TensorMacros.h
│   │           │       │   ├── TensorMap.h
│   │           │       │   ├── TensorMeta.h
│   │           │       │   ├── TensorMorphing.h
│   │           │       │   ├── TensorPadding.h
│   │           │       │   ├── TensorPatch.h
│   │           │       │   ├── TensorRandom.h
│   │           │       │   ├── TensorReduction.h
│   │           │       │   ├── TensorReductionCuda.h
│   │           │       │   ├── TensorReductionGpu.h
│   │           │       │   ├── TensorReductionSycl.h
│   │           │       │   ├── TensorRef.h
│   │           │       │   ├── TensorReverse.h
│   │           │       │   ├── TensorScan.h
│   │           │       │   ├── TensorScanSycl.h
│   │           │       │   ├── TensorShuffling.h
│   │           │       │   ├── TensorStorage.h
│   │           │       │   ├── TensorStriding.h
│   │           │       │   ├── TensorTrace.h
│   │           │       │   ├── TensorTraits.h
│   │           │       │   ├── TensorUInt128.h
│   │           │       │   └── TensorVolumePatch.h
│   │           │       ├── TensorSymmetry
│   │           │       │   ├── DynamicSymmetry.h
│   │           │       │   ├── StaticSymmetry.h
│   │           │       │   ├── Symmetry.h
│   │           │       │   └── util
│   │           │       │       └── TemplateGroupTheory.h
│   │           │       ├── ThreadPool
│   │           │       │   ├── Barrier.h
│   │           │       │   ├── EventCount.h
│   │           │       │   ├── NonBlockingThreadPool.h
│   │           │       │   ├── RunQueue.h
│   │           │       │   ├── ThreadCancel.h
│   │           │       │   ├── ThreadEnvironment.h
│   │           │       │   ├── ThreadLocal.h
│   │           │       │   ├── ThreadPoolInterface.h
│   │           │       │   └── ThreadYield.h
│   │           │       └── util
│   │           │           ├── CXX11Meta.h
│   │           │           ├── CXX11Workarounds.h
│   │           │           ├── EmulateArray.h
│   │           │           └── MaxSizeVector.h
│   │           ├── EulerAngles
│   │           ├── FFT
│   │           ├── IterativeSolvers
│   │           ├── KroneckerProduct
│   │           ├── LevenbergMarquardt
│   │           ├── MPRealSupport
│   │           ├── MatrixFunctions
│   │           ├── MoreVectorization
│   │           ├── NonLinearOptimization
│   │           ├── NumericalDiff
│   │           ├── OpenGLSupport
│   │           ├── Polynomials
│   │           ├── Skyline
│   │           ├── SparseExtra
│   │           ├── SpecialFunctions
│   │           ├── Splines
│   │           └── src
│   │               ├── AutoDiff
│   │               │   ├── AutoDiffJacobian.h
│   │               │   ├── AutoDiffScalar.h
│   │               │   └── AutoDiffVector.h
│   │               ├── BVH
│   │               │   ├── BVAlgorithms.h
│   │               │   └── KdBVH.h
│   │               ├── Eigenvalues
│   │               │   └── ArpackSelfAdjointEigenSolver.h
│   │               ├── EulerAngles
│   │               │   ├── EulerAngles.h
│   │               │   └── EulerSystem.h
│   │               ├── FFT
│   │               │   ├── ei_fftw_impl.h
│   │               │   └── ei_kissfft_impl.h
│   │               ├── IterativeSolvers
│   │               │   ├── ConstrainedConjGrad.h
│   │               │   ├── DGMRES.h
│   │               │   ├── GMRES.h
│   │               │   ├── IDRS.h
│   │               │   ├── IncompleteLU.h
│   │               │   ├── IterationController.h
│   │               │   ├── MINRES.h
│   │               │   └── Scaling.h
│   │               ├── KroneckerProduct
│   │               │   └── KroneckerTensorProduct.h
│   │               ├── LevenbergMarquardt
│   │               │   ├── LMcovar.h
│   │               │   ├── LMonestep.h
│   │               │   ├── LMpar.h
│   │               │   ├── LMqrsolv.h
│   │               │   └── LevenbergMarquardt.h
│   │               ├── MatrixFunctions
│   │               │   ├── MatrixExponential.h
│   │               │   ├── MatrixFunction.h
│   │               │   ├── MatrixLogarithm.h
│   │               │   ├── MatrixPower.h
│   │               │   ├── MatrixSquareRoot.h
│   │               │   └── StemFunction.h
│   │               ├── MoreVectorization
│   │               │   └── MathFunctions.h
│   │               ├── NonLinearOptimization
│   │               │   ├── HybridNonLinearSolver.h
│   │               │   ├── LevenbergMarquardt.h
│   │               │   ├── chkder.h
│   │               │   ├── covar.h
│   │               │   ├── dogleg.h
│   │               │   ├── fdjac1.h
│   │               │   ├── lmpar.h
│   │               │   ├── qrsolv.h
│   │               │   ├── r1mpyq.h
│   │               │   ├── r1updt.h
│   │               │   └── rwupdt.h
│   │               ├── NumericalDiff
│   │               │   └── NumericalDiff.h
│   │               ├── Polynomials
│   │               │   ├── Companion.h
│   │               │   ├── PolynomialSolver.h
│   │               │   └── PolynomialUtils.h
│   │               ├── Skyline
│   │               │   ├── SkylineInplaceLU.h
│   │               │   ├── SkylineMatrix.h
│   │               │   ├── SkylineMatrixBase.h
│   │               │   ├── SkylineProduct.h
│   │               │   ├── SkylineStorage.h
│   │               │   └── SkylineUtil.h
│   │               ├── SparseExtra
│   │               │   ├── BlockOfDynamicSparseMatrix.h
│   │               │   ├── BlockSparseMatrix.h
│   │               │   ├── DynamicSparseMatrix.h
│   │               │   ├── MarketIO.h
│   │               │   ├── MatrixMarketIterator.h
│   │               │   └── RandomSetter.h
│   │               ├── SpecialFunctions
│   │               │   ├── BesselFunctionsArrayAPI.h
│   │               │   ├── BesselFunctionsBFloat16.h
│   │               │   ├── BesselFunctionsFunctors.h
│   │               │   ├── BesselFunctionsHalf.h
│   │               │   ├── BesselFunctionsImpl.h
│   │               │   ├── BesselFunctionsPacketMath.h
│   │               │   ├── HipVectorCompatibility.h
│   │               │   ├── SpecialFunctionsArrayAPI.h
│   │               │   ├── SpecialFunctionsBFloat16.h
│   │               │   ├── SpecialFunctionsFunctors.h
│   │               │   ├── SpecialFunctionsHalf.h
│   │               │   ├── SpecialFunctionsImpl.h
│   │               │   ├── SpecialFunctionsPacketMath.h
│   │               │   └── arch
│   │               │       ├── AVX
│   │               │       │   ├── BesselFunctions.h
│   │               │       │   └── SpecialFunctions.h
│   │               │       ├── AVX512
│   │               │       │   ├── BesselFunctions.h
│   │               │       │   └── SpecialFunctions.h
│   │               │       ├── GPU
│   │               │       │   └── SpecialFunctions.h
│   │               │       └── NEON
│   │               │           ├── BesselFunctions.h
│   │               │           └── SpecialFunctions.h
│   │               └── Splines
│   │                   ├── Spline.h
│   │                   ├── SplineFitting.h
│   │                   └── SplineFwd.h
│   └── fusion\012    
│       └── imu_fusion.h
├── lib
│   ├── cmake
│   │   └── ImuProject
│   │       ├── ImuProjectConfig.cmake
│   │       ├── ImuProjectTargets-noconfig.cmake
│   │       └── ImuProjectTargets.cmake
│   ├── libdriver.a
│   └── libfusion.a
└── share
    ├── eigen3
    │   └── cmake
    │       ├── Eigen3Config.cmake
    │       ├── Eigen3ConfigVersion.cmake
    │       ├── Eigen3Targets.cmake
    │       └── UseEigen3.cmake
    └── pkgconfig
        └── eigen3.pc

92 directories, 542 files
➜  class05_export git:(main) ✗ 
```