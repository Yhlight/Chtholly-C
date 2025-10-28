cmake_minimum_required(VERSION 3.10)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find llvm-config
find_program(LLVM_CONFIG "llvm-config-16")
if(NOT LLVM_CONFIG)
    message(FATAL_ERROR "llvm-config-16 not found. Please install LLVM 16 and add it to your PATH.")
endif()

# Get include dirs
execute_process(
    COMMAND ${LLVM_CONFIG} --includedir
    OUTPUT_VARIABLE LLVM_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get link flags
execute_process(
    COMMAND ${LLVM_CONFIG} --libs --system-libs all
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

include_directories(${LLVM_INCLUDE_DIR})

# Add source files
add_executable(chtholly src/main.cpp src/Lexer.cpp src/Parser.cpp src/CodeGen.cpp)
target_link_libraries(chtholly PRIVATE ${LLVM_LIBS})


# Add tests
enable_testing()
add_executable(lexer_test tests/LexerTest.cpp src/Lexer.cpp)
target_link_libraries(lexer_test PRIVATE ${LLVM_LIBS})
add_test(NAME lexer_test COMMAND lexer_test)


add_executable(parser_test tests/ParserTest.cpp src/Lexer.cpp src/Parser.cpp)
target_link_libraries(parser_test PRIVATE ${LLVM_LIBS})
add_test(NAME parser_test COMMAND parser_test)

add_executable(codegen_test tests/CodeGenTest.cpp src/Lexer.cpp src/Parser.cpp src/CodeGen.cpp)
target_link_libraries(codegen_test PRIVATE ${LLVM_LIBS})
add_test(NAME codegen_test COMMAND codegen_test)
