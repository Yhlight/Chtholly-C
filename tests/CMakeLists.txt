include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(chtholly_tests main_test.cpp)
target_link_libraries(chtholly_tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(chtholly_tests)
