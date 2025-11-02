cmake_minimum_required(VERSION 3.10)

project(ChthollyTests)

add_executable(
    ChthollyTests
    test_lexer.cpp
    test_parser.cpp
    test_transpiler.cpp
)

target_link_libraries(
    ChthollyTests
    GTest::gtest_main
    chtholly_lib
)

include(GoogleTest)
gtest_discover_tests(ChthollyTests)
