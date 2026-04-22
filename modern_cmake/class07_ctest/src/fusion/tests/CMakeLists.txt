# src/fusion/tests/CMakeLists.txt 用于构建测试可执行程序
add_executable(test_fusion test_fusion.cpp)

# 这个可执行程序只是为了测试fusion库, 所以也只依赖这个库, 也只需要私有权限
target_link_libraries(test_fusion PRIVATE fusion)

# 注册成测试
add_test(NAME fusion_test COMMAND test_fusion)
# 超时限制 30s
set_tests_properties(fusion_test PROPERTIES TIMEOUT 30)
