add_library(chtholly_lib STATIC
    Chtholly.cpp
    Chtholly.h
    Error.cpp
    Error.h
    Token.h
    Lexer.cpp
    Lexer.h
    Parser.cpp
    Parser.h
    Resolver.cpp
    Resolver.h
    Transpiler.cpp
    Transpiler.h
    AST/Expr.h
    AST/Stmt.h
    AST/ASTPrinter.cpp
    AST/ASTPrinter.h
)

target_include_directories(chtholly_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/AST
)

add_executable(chtholly main.cpp)

target_link_libraries(chtholly PRIVATE chtholly_lib)
