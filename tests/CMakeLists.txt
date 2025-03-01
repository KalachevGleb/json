cmake_minimum_required(VERSION 3.10)

# Set the project name
project(JSONTests)

# Add GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Fetch and add GoogleTest
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Список всех тестовых файлов
set(TEST_SOURCES
    json_test.cpp
    string_escape_test.cpp
    formatting_test.cpp
    preferred_size_test.cpp
    comparison_test.cpp
    parser_test.cpp
    coverage_test.cpp
)

# Add the test executable
add_executable(
  json_test
  ${TEST_SOURCES}
)

# Include the json directory
include_directories(${CMAKE_SOURCE_DIR}/../json)

# Link the test executable with GoogleTest and the JSON library
 target_link_libraries(
  json_test
  gtest_main
  json
)

# Add the tests
include(GoogleTest)
gtest_discover_tests(json_test)
