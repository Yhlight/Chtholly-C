cmake_minimum_required(VERSION 3.10)
project(Chtholly CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
