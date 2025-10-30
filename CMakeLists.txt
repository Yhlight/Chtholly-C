cmake_minimum_required(VERSION 3.10)
project(Chtholly CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

# We will enable testing later when we have a testing framework
enable_testing()
add_subdirectory(tests)
