cmake_minimum_required(VERSION 3.10)
project(ChthollyTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG    release-1.12.1
)

FetchContent_MakeAvailable(googletest)

add_executable(chtholly_tests
  LexerTest.cpp
  ParserTest.cpp
)

target_link_libraries(chtholly_tests
  PRIVATE
  GTest::gtest_main
  chtholly_lib
)

include(GoogleTest)
gtest_discover_tests(chtholly_tests)
