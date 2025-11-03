cmake_minimum_required(VERSION 3.10)

# Add the Google Test subdirectory
add_subdirectory(lib/googletest)

add_executable(ChthollyTests
    LexerTests.cpp
)

target_link_libraries(ChthollyTests
    PRIVATE
    ChthollyLib
    gtest_main
)

# Include the Google Test directory so we can find gtest/gtest.h
include_directories(lib/googletest/googletest/include)
