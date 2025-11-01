enable_testing()

add_executable(chtholly_tests
    TestHelpers.cpp
    LexerTest.cpp
    ParserTest.cpp
    SymbolTableTest.cpp
    SemaTest.cpp
    CodeGenTest.cpp
    VariableTest.cpp
    ArrayTest.cpp
)

target_link_libraries(chtholly_tests PRIVATE chtholly_lib)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

target_link_libraries(chtholly_tests PRIVATE gtest_main)
