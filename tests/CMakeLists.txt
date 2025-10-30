include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(chtholly_tests test_codegen.cpp ../src/cli.cpp ../src/Lexer.cpp ../src/Parser.cpp ../src/CodeGen.cpp)
target_link_libraries(chtholly_tests GTest::gtest_main)

target_include_directories(chtholly_tests PUBLIC ../src)

include(GoogleTest)
gtest_discover_tests(chtholly_tests)
