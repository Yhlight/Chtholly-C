cmake_minimum_required(VERSION 3.10)
project(Chtholly)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(chtholly src/main.cpp src/Token.cpp src/Lexer.cpp src/AstPrinter.cpp src/Parser.cpp src/Environment.cpp src/SemanticAnalyzer.cpp src/CodeGen.cpp)
