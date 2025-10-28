cmake_minimum_required(VERSION 3.16)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 16 REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(chtholly src/main.cpp)

target_link_libraries(chtholly PRIVATE ${LLVM_LIBRARIES})
