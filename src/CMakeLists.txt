add_library(chtholly_lib STATIC
    # Add source files here
    main.cpp
)

target_include_directories(chtholly_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
