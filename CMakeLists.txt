cmake_minimum_required(VERSION 3.16)

project(HTTPSocketServer)

add_executable(server.run main.cpp)

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

target_compile_options(server.run PRIVATE ${WARNING_FLAGS} ${LANG_STANDARD})


add_executable(client.run client.cpp)

target_compile_options(client.run PRIVATE ${WARNING_FLAGS} ${LANG_STANDARD})


# Database file installation
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/database/db.txt
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
