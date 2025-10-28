cmake_minimum_required(VERSION 3.10)
project(ChthollyTests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(run_tests main.cpp)

add_executable(lexer_test lexer_test.cpp ../src/Lexer.cpp)

add_executable(parser_test parser_test.cpp ../src/Parser.cpp ../src/Lexer.cpp)

add_test(NAME run_tests COMMAND run_tests)
add_test(NAME lexer_test COMMAND lexer_test)
add_test(NAME parser_test COMMAND parser_test)
