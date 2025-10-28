cmake_minimum_required(VERSION 3.10)
project(ChthollyTests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(run_tests main.cpp)

add_executable(lexer_test lexer_test.cpp ../src/Lexer.cpp)

add_executable(parser_test parser_test.cpp ../src/Parser.cpp ../src/Lexer.cpp)

add_executable(sema_test sema_test.cpp ../src/Sema.cpp ../src/Parser.cpp ../src/Lexer.cpp ../src/SymbolTable.cpp)

add_executable(type_test type_test.cpp ../src/Sema.cpp ../src/Parser.cpp ../src/Lexer.cpp ../src/SymbolTable.cpp)

add_test(NAME run_tests COMMAND run_tests)
add_test(NAME lexer_test COMMAND lexer_test)
add_test(NAME parser_test COMMAND parser_test)
add_test(NAME sema_test COMMAND sema_test)
add_test(NAME type_test COMMAND type_test)
