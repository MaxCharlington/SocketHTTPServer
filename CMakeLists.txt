cmake_minimum_required(VERSION 3.16)

project(HTTPSocketServer)

set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_compile_options(@${PROJECT_SOURCE_DIR}/compiler_flags.txt)

add_subdirectory(samples)
add_subdirectory(tests)
