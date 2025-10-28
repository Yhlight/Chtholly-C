cmake_minimum_required(VERSION 3.16)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 16 REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(src/AST)
add_definitions(${LLVM_DEFINITIONS})

add_executable(chtholly
    src/main.cpp
    src/Lexer.cpp
    src/Parser.cpp
)

target_link_libraries(chtholly PRIVATE ${LLVM_LIBRARIES})

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_subdirectory(tests)
