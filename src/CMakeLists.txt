add_library(chtholly_lib
    main.cpp
    Token.cpp
    Lexer.cpp
    AST/ASTPrinter.cpp
    Parser.cpp
)

target_include_directories(chtholly_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
