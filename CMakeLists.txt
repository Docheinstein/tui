cmake_minimum_required(VERSION 3.25)
project(tui)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)

add_executable(tui-example)

target_sources(tui-example PRIVATE main.cpp)

target_link_libraries(tui-example PRIVATE tui)
