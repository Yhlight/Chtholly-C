add_library(chtholly_lib)

# Add source files to the library
target_sources(chtholly_lib PRIVATE
    Lexer.cpp
    Parser.cpp
    SymbolTable.cpp
    Sema.cpp
)

# Expose the 'src' directory to consumers of this library, so they can find headers.
target_include_directories(chtholly_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
