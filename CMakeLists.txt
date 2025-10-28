cmake_minimum_required(VERSION 3.10)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM
find_package(LLVM 16 REQUIRED CONFIG)

# Add LLVM include directories
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Add the executable
add_executable(Chtholly src/main.cpp)

# Link against LLVM libraries
target_link_libraries(Chtholly PRIVATE ${LLVM_LIBRARIES})

# Add CXX flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXX_FLAGS}")

# Add tests
enable_testing()
add_executable(simple_test tests/simple_test.cpp)
add_test(NAME simple_test COMMAND simple_test)
