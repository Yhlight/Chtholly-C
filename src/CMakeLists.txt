add_library(chtholly_lib
    Lexer.cpp
    Error.cpp
    AST/ASTPrinter.cpp
    Parser.cpp
)

target_include_directories(chtholly_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
