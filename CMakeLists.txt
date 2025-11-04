cmake_minimum_required(VERSION 3.10)
project(Chtholly)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_executable(chtholly src/main.cpp src/Lexer.cpp src/Parser.cpp src/ASTPrinter.cpp src/Environment.cpp src/SemanticAnalyzer.cpp src/CodeGen.cpp)
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

add_executable(test_codegen tests/test_codegen.cpp src/CodeGen.cpp)
target_include_directories(test_codegen PUBLIC include)
add_test(NAME CodeGenTest COMMAND test_codegen)

add_executable(test_control_flow tests/test_control_flow.cpp src/Lexer.cpp src/Parser.cpp src/SemanticAnalyzer.cpp src/CodeGen.cpp src/Environment.cpp)
target_include_directories(test_control_flow PUBLIC include)
add_test(NAME ControlFlowTest COMMAND test_control_flow)

add_executable(test_functions tests/test_functions.cpp src/Lexer.cpp src/Parser.cpp src/SemanticAnalyzer.cpp src/CodeGen.cpp src/Environment.cpp)
target_include_directories(test_functions PUBLIC include)
add_test(NAME FunctionTest COMMAND test_functions)

add_executable(test_data_types tests/test_data_types.cpp src/Lexer.cpp src/Parser.cpp)
target_include_directories(test_data_types PUBLIC include)
add_test(NAME DataTypeTest COMMAND test_data_types)

add_executable(test_logical_operators tests/test_logical_operators.cpp src/Lexer.cpp src/Parser.cpp src/ASTPrinter.cpp)
target_include_directories(test_logical_operators PUBLIC include)
add_test(NAME LogicalOperatorTest COMMAND test_logical_operators)

add_executable(test_structs tests/test_structs.cpp src/Lexer.cpp src/Parser.cpp src/CodeGen.cpp)
target_include_directories(test_structs PUBLIC include)
add_test(NAME StructTest COMMAND test_structs)

add_executable(test_ownership tests/test_ownership.cpp src/Lexer.cpp src/Parser.cpp src/CodeGen.cpp)
target_include_directories(test_ownership PUBLIC include)
add_test(NAME OwnershipTest COMMAND test_ownership)
