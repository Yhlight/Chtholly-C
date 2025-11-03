# Add the main Chtholly source files to a library
add_library(ChthollyLib STATIC
    Chtholly.cpp
    Chtholly.h
    Lexer.cpp
    Lexer.h
    Token.cpp
    Token.h
    TokenType.h
)

# Target include directories for Chtholly library
target_include_directories(ChthollyLib PUBLIC .)

# Add the main executable
add_executable(Chtholly main.cpp)

# Link the executable against the Chtholly library
target_link_libraries(Chtholly PRIVATE ChthollyLib)
