cmake_minimum_required(VERSION 3.10)
project(Chtholly)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_executable(chtholly src/main.cpp src/Lexer.cpp)
target_include_directories(chtholly PUBLIC include)

add_executable(test_lexer tests/test_lexer.cpp src/Lexer.cpp)
target_include_directories(test_lexer PUBLIC include)
add_test(NAME LexerTest COMMAND test_lexer)
