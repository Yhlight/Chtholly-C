add_library(chtholly_lib
    Lexer.cpp
    Parser.cpp
    SymbolTable.cpp
    Sema.cpp
    CodeGen.cpp
    Filesystem.cpp
)

target_include_directories(chtholly_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
