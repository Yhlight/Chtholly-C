cmake_minimum_required(VERSION 3.10)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Chtholly library
add_library(chtholly_lib
    src/Token.h
    src/Lexer.h src/Lexer.cpp
    src/AST/Expr.h
    src/AST/Stmt.h
    src/AST/ASTPrinter.h src/AST/ASTPrinter.cpp
    src/Parser.h src/Parser.cpp
    src/Transpiler.h src/Transpiler.cpp
    src/Type.h src/Type.cpp
)

add_executable(chtholly src/main.cpp)
target_link_libraries(chtholly PRIVATE chtholly_lib)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

# Test executable
add_executable(chtholly_tests
    tests/test_main.cpp
    tests/test_lexer.cpp
    tests/test_parser.cpp
    tests/test_transpiler.cpp
)
target_link_libraries(chtholly_tests PRIVATE GTest::gtest_main chtholly_lib)
