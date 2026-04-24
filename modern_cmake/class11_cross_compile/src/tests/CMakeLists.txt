# GeoUtils/src/tests/CMakeLists.txt

# core单元测试
add_executable(test_core "${CMAKE_CURRENT_SOURCE_DIR}/test_core.cpp")
target_link_libraries(test_core PRIVATE geo_core)
# 添加测试和时间限制
add_test(NAME core_test COMMAND $<TARGET_FILE:test_core>)
set_tests_properties(core_test PROPERTIES TIMEOUT 10)

# algo单元测试
add_executable(test_algo "${CMAKE_CURRENT_SOURCE_DIR}/test_algo.cpp")
target_link_libraries(test_algo PRIVATE geo_algo)
# 添加测试和时间限制
add_test(NAME algo_test COMMAND $<TARGET_FILE:test_algo>)
set_tests_properties(algo_test PROPERTIES TIMEOUT 10)
