add_executable(chtholly_tests
    LexerTests.cpp
    ParserTests.cpp
    ResolverTests.cpp
    TranspilerTests.cpp
)

target_link_libraries(chtholly_tests PRIVATE chtholly_lib gtest_main)

include(GoogleTest)
gtest_discover_tests(chtholly_tests)
