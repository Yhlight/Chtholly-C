include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG    release-1.11.0
)

FetchContent_MakeAvailable(googletest)

add_executable(chtholly_tests
    # Add test source files here
    main_test.cpp
)

target_link_libraries(chtholly_tests PRIVATE chtholly_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(chtholly_tests)
