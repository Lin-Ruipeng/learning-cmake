add_executable(app "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

target_link_libraries(app PRIVATE geo_algo)
