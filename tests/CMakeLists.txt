add_executable(chtholly_tests
    ErrorReporterTests.cpp
    LexerTests.cpp
    ASTPrinterTests.cpp
    ParserTests.cpp
    TranspilerTests.cpp
)

target_link_libraries(chtholly_tests
    PRIVATE
        chtholly_lib
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(chtholly_tests)
