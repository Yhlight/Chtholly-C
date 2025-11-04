add_library(chtholly_lib
    common/Error.cpp
    frontend/AST/ASTPrinter.cpp
    frontend/Lexer.cpp
    frontend/Parser.cpp
    frontend/PrattParser.cpp
    middle/Resolver.cpp
    backend/Transpiler.cpp
    driver/Chtholly.cpp
)

target_include_directories(chtholly_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(chtholly driver/main.cpp)
target_link_libraries(chtholly chtholly_lib)
