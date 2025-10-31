add_library(chtholly_lib
    Token.cpp
    Lexer.cpp
    AST/ASTPrinter.cpp
    Parser.cpp
    SymbolTable.cpp
    Resolver.cpp
    Transpiler.cpp
)

target_include_directories(chtholly_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
