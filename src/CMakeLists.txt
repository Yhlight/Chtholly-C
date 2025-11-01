add_library(chtholly_lib STATIC
    Chtholly.cpp
    Chtholly.h
)

target_include_directories(chtholly_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(chtholly main.cpp)

target_link_libraries(chtholly PRIVATE chtholly_lib)
