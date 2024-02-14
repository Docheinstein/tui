cmake_minimum_required(VERSION 3.17)
project(tui)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
add_subdirectory(include)

option(TUI_BUILD_EXAMPLES "Build TUI examples" OFF)

if (TUI_BUILD_EXAMPLES)
    add_executable(tui-example)
    target_sources(tui-example PRIVATE example/main.cpp)
    target_link_libraries(tui-example PRIVATE tui)
endif ()