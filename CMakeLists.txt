cmake_minimum_required(VERSION 3.10)

project(chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chtholly src/main.cpp)

# Include directories
target_include_directories(chtholly PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
