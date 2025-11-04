cmake_minimum_required(VERSION 3.10)
project(Chtholly)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_executable(chtholly src/main.cpp src/Lexer.cpp src/Parser.cpp src/ASTPrinter.cpp src/Environment.cpp src/SemanticAnalyzer.cpp)
target_include_directories(chtholly PUBLIC include)

add_executable(test_lexer tests/test_lexer.cpp src/Lexer.cpp)
target_include_directories(test_lexer PUBLIC include)
add_test(NAME LexerTest COMMAND test_lexer)

add_executable(test_parser tests/test_parser.cpp src/Lexer.cpp src/Parser.cpp src/ASTPrinter.cpp)
target_include_directories(test_parser PUBLIC include)
add_test(NAME ParserTest COMMAND test_parser)

add_executable(test_semantic_analyzer tests/test_semantic_analyzer.cpp src/Lexer.cpp src/Parser.cpp src/Environment.cpp src/SemanticAnalyzer.cpp)
target_include_directories(test_semantic_analyzer PUBLIC include)
add_test(NAME SemanticAnalyzerTest COMMAND test_semantic_analyzer)
