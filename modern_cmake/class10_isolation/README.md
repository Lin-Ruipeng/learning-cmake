# 构建命令
```bash
cmake --preset debug
cmake --build --preset build-debug
ctest --preset test-debug
# 安装时要注意, 用debug模型构建的文件都在 build/debug 目录下!路径要对应
cmake --install build/debug --prefix ./install 
tree ./install
```
