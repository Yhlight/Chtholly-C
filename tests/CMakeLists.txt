add_executable(chtholly_tests main.cpp)

target_link_libraries(chtholly_tests PRIVATE chtholly_lib gtest_main)

include(GoogleTest)
gtest_discover_tests(chtholly_tests)
