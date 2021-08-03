cmake_minimum_required(VERSION 3.15)
project(server DESCRIPTION "core of project beef's gtps" VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(CPR_BUILD_TESTS OFF)
set(JSON_BuildTests OFF)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(vendor)
