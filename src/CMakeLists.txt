# Create a library with the core logic
add_library(chtholly_lib cli.cpp Lexer.cpp Parser.cpp)

# Publicly expose the include directory for the library
target_include_directories(chtholly_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Create the main executable
add_executable(chtholly main.cpp)

# Link the executable against the library
target_link_libraries(chtholly chtholly_lib)
