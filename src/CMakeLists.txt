add_library(chtholly_lib STATIC
    Chtholly.cpp
    Lexer.cpp
    AstPrinter.cpp
    Parser.cpp
    Transpiler.cpp
    Resolver.cpp
    SymbolTable.cpp
)

target_include_directories(chtholly_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(chtholly_lib PUBLIC CXX_COMPILER="${CMAKE_CXX_COMPILER}")

add_executable(chtholly
    main.cpp
)

target_link_libraries(chtholly PRIVATE chtholly_lib)
