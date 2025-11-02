add_library(chtholly_lib
    TypeInfo.h
    Lexer.cpp
    Error.cpp
    AST/ASTPrinter.cpp
    Parser.cpp
    Transpiler.cpp
    Chtholly.cpp
    Resolver.cpp
)

target_include_directories(chtholly_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(chtholly main.cpp)
target_link_libraries(chtholly chtholly_lib)
