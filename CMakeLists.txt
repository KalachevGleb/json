cmake_minimum_required(VERSION 3.12)

project(example)

# Enable testing support
enable_testing()
include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add coverage flags for Clang
option(CODE_COVERAGE "Enable coverage reporting" OFF)
if(CODE_COVERAGE)
    message("Coverage enabled")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0)
        add_link_options(--coverage)
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
        add_link_options(-fprofile-instr-generate)
    endif()
endif()

add_subdirectory(json)
add_subdirectory(tests)
add_executable(example example/example.cpp)
target_link_libraries(example json)
