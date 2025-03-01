cmake_minimum_required(VERSION 3.12)

project(example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(json)
add_subdirectory(tests)
add_executable(example example/example.cpp)
target_link_libraries(example json)
