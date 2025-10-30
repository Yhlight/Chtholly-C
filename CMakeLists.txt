cmake_minimum_required(VERSION 3.10)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chtholly src/main.cpp)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

# Test executable
add_executable(chtholly_tests tests/test_main.cpp)
target_link_libraries(chtholly_tests PRIVATE GTest::gtest_main)
