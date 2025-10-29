cmake_minimum_required(VERSION 3.16)

add_executable(run_tests
    LexerTest.cpp
    ParserTest.cpp
    ForLoopTest.cpp
    SwitchStatementTest.cpp
    ../src/Lexer.cpp
    ../src/Parser.cpp
)

target_include_directories(run_tests PRIVATE ../src)

target_link_libraries(run_tests PRIVATE
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(run_tests)
