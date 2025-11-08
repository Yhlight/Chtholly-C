include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(chtholly_tests
    main.cpp
    LexerTests.cpp
    AstPrinterTests.cpp
    ParserTests.cpp
    TranspilerTests.cpp
    MetaTests.cpp
    ReflectionTests.cpp
    UtilTests.cpp
    TestOperatorOverload.cpp
    TestGenerics.cpp
    TestArray.cpp
    TestEnum.cpp
    ImportTests.cpp
    TestHelpers.cpp
)

target_link_libraries(chtholly_tests PRIVATE chtholly_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(chtholly_tests)
