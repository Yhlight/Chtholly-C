cmake_minimum_required(VERSION 3.10)

project(ChthollyTests)

add_executable(
    ChthollyTests
    test_error_reporting.cpp
    test_lexer.cpp
    test_parser.cpp
    test_transpiler.cpp
    test_resolver.cpp
    test_operator_overload.cpp
    test_simple_trait.cpp
    test_traits.cpp
    test_modules.cpp
    test_std_import.cpp
    test_filesystem.cpp
    test_switch.cpp
    test_reflect.cpp
    test_meta.cpp
)

target_link_libraries(
    ChthollyTests
    GTest::gtest_main
    chtholly_lib
)

include(GoogleTest)
gtest_discover_tests(ChthollyTests)
