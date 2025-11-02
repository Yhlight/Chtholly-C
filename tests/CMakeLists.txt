add_executable(chtholly_tests
    ErrorReporterTests.cpp
    LexerTests.cpp
    ASTPrinterTests.cpp
    ParserTests.cpp
    TranspilerTests.cpp
        SwitchTests.cpp
    ResolverTests.cpp
    FunctionTests.cpp
    StructTests.cpp
)

target_link_libraries(chtholly_tests
    PRIVATE
        chtholly_lib
        gtest_main
)

include(GoogleTest)
add_test(NAME chtholly_tests COMMAND chtholly_tests)
