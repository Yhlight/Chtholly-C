cmake_minimum_required(VERSION 3.10)

project(ChthollyTests)

add_executable(
    ChthollyTests
    test_lexer.cpp
    test_parser.cpp
    test_transpiler.cpp
    test_resolver.cpp
    test_traits.cpp
    test_modules.cpp
    test_std_import.cpp
    test_filesystem.cpp
)

target_link_libraries(
    ChthollyTests
    GTest::gtest_main
    chtholly_lib
)

include(GoogleTest)
gtest_discover_tests(ChthollyTests)
