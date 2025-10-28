cmake_minimum_required(VERSION 3.10)
project(Chtholly C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM
find_package(LLVM 16 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM library dir: ${LLVM_LIBRARY_DIR}")

# Add LLVM include directories and definitions
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(chthollyc src/main.cpp)

# Get the list of libraries for the specified components
llvm_map_components_to_libnames(chtholly_libs core support)

# Link to LLVM libraries
target_link_libraries(chthollyc PRIVATE ${chtholly_libs})

enable_testing()
add_subdirectory(tests)
