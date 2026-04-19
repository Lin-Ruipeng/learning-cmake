# 交叉编译

## 编译前准备

除去原有的 `CMakeLists.txt` 你还需要创建一个专门用于导入交叉编译器的 `xxx.camke` 文件
最重要的是需要指定好编译器的位置, 也就是:

```cmake
set(CMAKE_C_COMPILER "gcc交叉编译器路径")
set(CMAKE_CXX_COMPILER "g++交叉编译器路径")
```

这里的编译器需要自行去对应板子的官网上下载!

## 编译命令(重要)

```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=./xxx.cmake
cmake --build build
```

## 补充
编译完成之后的操作流程：

1. 首先需要将编译后的文件拷贝到win下:
```bash
cp ./build/app /mnt/c/Users/MuXinYue/Desktop/GCC 
```

2. 然后需要通过 `scp` 命令把这个文件通过ssh隧道拷贝到板子上
(先确保用 USB 线连接到板子并且可以成功用ssh连接!)
```bash
scp app pico@172.32.0.70:/home/pico/
```
> 总结就是 scp 源文件地址 目标目录地址
> 然后需要注意的就是 目标目录地址是 用户名@IP:路径
然后根据提示输入密码 ssh 即可

3. 用 ssh 登录, 给文件赋予可执行权限!
```bash
ssh pico@172.32.0.70
# 然后需要输入登录密码
# 登录到的目录应该就是 /home/pico 
# 只需要给拥有者 用户(user) 可执行权限
chomd u+x app
``` 

4. 执行程序看到成功运行!
```bash
./app
```

