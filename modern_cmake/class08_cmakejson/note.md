### 第八课：构建流程标准化 —— 拥抱 CMakePresets.json
在前七课中，你已经拥有了一个功能完备、架构现代的 C++ 工程库。但想象一下，当你把代码推到 Git 仓库，一个新同事拉取代码后，你需要在微信里告诉他：
“哥们，先建个 build 目录，然后运行 `cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_EXTRA_WARNINGS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON`，接着用 `cmake --build build -j8` 编译，最后用 `ctest --test-dir build --output-on-failure` 跑测试。”
这太痛苦了！
1. 命令太长，容易敲错。
2. 团队里有人用 Ninja，有人用 Makefiles，有人用 VS，生成器怎么统一？
3. 如果以后加了个新的 Option，又要全员通知修改命令行。
为了解决这个问题，CMake 官方在 3.19 版本引入了**划时代的特性**：`CMakePresets.json`。它的目标是：**彻底消灭手敲 CMake 命令行，实现“一键构建”。**
#### 核心概念：JSON 驱动的配置继承链
`CMakePresets.json` 是一个放在项目根目录的严格 JSON 文件。它通过“预设”来封装复杂的命令行参数，并且支持**继承**。
1.  **`configurePresets`**：对应 `cmake -B ... -D...`（配置阶段）。
2.  **`buildPresets`**：对应 `cmake --build ...`（编译阶段）。
3.  **`testPresets`**：对应 `ctest ...`（测试阶段）。
4.  **`"hidden": true`**：这是一个神级属性。你可以写一个名为 `base` 的 preset，把所有公共的、底层的配置放进去，并标记为 hidden。它不会在列表里显示，但可以被其他 preset 继承（通过 `"inherits": "base"`）。
配置好之后，新同事只需要敲：
```bash
cmake --preset debug    # 自动执行配置
cmake --build --preset debug # 自动执行编译
ctest --preset debug    # 自动执行测试
```
---
### 实操出题：编写团队的标准化构建预设
**场景描述**：
你需要为项目编写一套标准的 `CMakePresets.json`，满足日常开发、发版和 IDE 跳转的需求。
**你的任务**：
在项目根目录创建 `CMakePresets.json` 文件。
**具体要求**：
1.  **基础隐藏预设 `base`**（`configurePresets`）：
    *   设置 `"hidden": true`。
    *   指定生成器为 `"Unix Makefiles"`（为了跨平台练习，先写死这个）。
    *   设置 `"CMAKE_EXPORT_COMPILE_COMMANDS": "ON"`（这对于 VSCode/CLion 的代码跳转和补全至关重要，必须开启）。
    *   设置构建目录为 `${sourceDir}/build/${presetName}`（这样 debug 和 release 的构建产物会自动隔离在不同文件夹，互不干扰）。
2.  **开发调试预设 `debug`**（`configurePresets`）：
    *   `"inherits": "base"`。
    *   `"displayName": "Debug 开发模式"`。
    *   设置 `"CMAKE_BUILD_TYPE": "Debug"`。
    *   设置 `"ENABLE_EXTRA_WARNINGS": "ON"`。
3.  **发布预设 `release`**（`configurePresets`）：
    *   `"inherits": "base"`。
    *   `"displayName": "Release 发布模式"`。
    *   设置 `"CMAKE_BUILD_TYPE": "Release"`。
    *   （注意：release 模式下不要手动设置 `ENABLE_EXTRA_WARNINGS`，让它走 CMakeLists.txt 里的默认值 ON 即可）。
4.  **编译预设 `build-debug`**（`buildPresets`）：
    *   `"configurePreset": "debug"`（关联到上面的 debug 配置）。
    *   设置并行编译参数 `"jobs": 8`。
5.  **测试预设 `test-debug`**（`testPresets`）：
    *   `"configurePreset": "debug"`。
    *   设置 `"output": {"output_on_failure": true}`（这样测试失败时，会直接打印出 `std::cout` 的内容，不用再手动加 `--output-on-failure`）。
**JSON 格式提示**：
*   所有的 Key 必须用**双引号**包裹。
*   布尔值是小写的 `true` / `false`，**没有引号**。
*   字符串值必须用**双引号**包裹（包括 `"ON"`）。
*   最后一个元素后面**不能有逗号**（严格 JSON 语法）。
请提交根目录下的 `CMakePresets.json` 文件内容。等待审查。

---

完全理解。`CMakePresets.json` 是 CMake 近年来引入的最庞大、最复杂的 JSON 语法体系，要求你第一次就手写出来确实强人所难。
它的核心难点在于：**严格的 JSON 语法限制**（不能有尾随逗号、必须双引号）加上**嵌套的面向对象继承逻辑**。
我不让你猜了。我直接给你一个**完全符合题目要求、可以直接复制运行的标准答案**，并附上逐行翻译。你现在的任务是**直接提交这段 JSON**，然后在本地体验它带来的魔法。
### 标准答案：`CMakePresets.json`
请在项目根目录创建文件 `CMakePresets.json`，**一字不差**地填入以下内容（注意：为了兼容你之前设置的 CMake 3.20，这里我剔除了注释，确保不会因为语法问题报错）：
```json
{
  "version": 6,
  "configurePresets": [
    {
      "name": "base",
      "hidden": true,
      "generator": "Unix Makefiles",
      "binaryDir": "${sourceDir}/build/${presetName}",
      "cacheVariables": {
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    },
    {
      "name": "debug",
      "displayName": "Debug 开发模式",
      "inherits": "base",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "ENABLE_EXTRA_WARNINGS": "ON"
      }
    },
    {
      "name": "release",
      "displayName": "Release 发布模式",
      "inherits": "base",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release"
      }
    }
  ],
  "buildPresets": [
    {
      "name": "build-debug",
      "configurePreset": "debug",
      "jobs": 8
    }
  ],
  "testPresets": [
    {
      "name": "test-debug",
      "configurePreset": "debug",
      "output": {
        "output_on_failure": true
      }
    }
  ]
}
```
---
### 保姆级结构拆解（请读完，这关乎你以后能不能自己改）
虽然文件里不能写注释，但我在这里给你拆解这 5 个区块的作用：
**1. `"version": 6`**
这是 CMake Presets 的 schema 版本号。目前主流推荐写 6（支持大部分好用的新特性，且兼容 CMake 3.20+）。
**2. `base` 区块（底层隐身基类）**
```json
"hidden": true  // 隐藏它，别人敲 cmake --list-presets 时看不到它
"generator": "Unix Makefiles"  // 强制指定生成器
"binaryDir": "${sourceDir}/build/${presetName}"  // 神级变量：源码目录下/build/预设名。这保证了 debug 编译在 build/debug，release 在 build/release，绝不互相污染。
"CMAKE_EXPORT_COMPILE_COMMANDS": "ON"  // 开启跳转补全数据库
```
**3. `debug` 与 `release` 区块（派生类）**
```json
"inherits": "base"  // 面向对象继承！它们自动获得了 base 里的生成器、输出目录等所有配置
"cacheVariables": { "CMAKE_BUILD_TYPE": "Debug" }  // 相当于命令行的 -D...
```
**4. `build-debug` 区块（编译快捷方式）**
```json
"configurePreset": "debug"  // 绑定配置预设。执行这行时，如果没配置过，会自动先执行 debug 的配置流程
"jobs": 8  // 相当于 make -j8，并行编译
```
**5. `test-debug` 区块（测试快捷方式）**
```json
"configurePreset": "debug",
"output": { "output_on_failure": true }  // 相当于 ctest --output-on-failure
```
---
### 见证魔法的时刻
提交这个文件后，你可以把之前的 `build` 文件夹删掉（`rm -rf build`），然后体验全新的人类友好级命令：
```bash
# 1. 一键配置 + 编译（底层自动帮你建目录、选生成器、传参数）
cmake --preset build-debug
# 2. 一键测试
ctest --preset test-debug
```
你会发现终端输出和以前手敲长串命令一模一样，但你再也不用记那些恶心的参数了。在 VSCode 或 CLion 里，它们也会自动识别这个 JSON 文件，在右下角给你提供一个下拉菜单让你直接点选切换 `debug/release`。
请直接提交上面的 JSON 内容完成第八课。

# 运行命令
经过测试，以下命令可以运行
```bash
cmake --preset debug
cmake --preset release
cmake --build --preset build-debug
ctest --preset test-debug
```