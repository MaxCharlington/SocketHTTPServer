cmake_minimum_required(VERSION 3.16)

project(HTTPSocketServer)

set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(WARNING_FLAGS
    -Wall
    -Wextra
    -Wshadow
    -Wno-write-strings
)

# set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")

# Leave it here for CMake 3.2 when it will be defaulted
# set_property(TARGET server PROPERTY CXX_STANDARD 23)
set(LANG_STANDARD -std=gnu++2b)  # Replacement

add_subdirectory(samples)
add_subdirectory(tests)
