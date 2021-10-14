cmake_minimum_required(VERSION 3.16)

project(HTTPSocketServer)

add_executable(server main.cpp)

set(WARNING_FLAGS
    -Wall
    -Wextra
    -Wshadow
)

# Leave it here for CMake 3.2 when it will be defaulted
# set_property(TARGET server PROPERTY CXX_STANDARD 23)
set(LANG_STANDARD -std=gnu++2b)  # Replacement

target_compile_options(server PRIVATE ${WARNING_FLAGS} ${LANG_STANDARD})
