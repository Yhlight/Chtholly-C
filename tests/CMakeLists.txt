add_executable(chtholly_tests
    main.cpp
    LexerTests.cpp
    ParserTests.cpp
)

target_link_libraries(chtholly_tests PRIVATE chtholly_lib gtest_main)

include(GoogleTest)
gtest_discover_tests(chtholly_tests)
