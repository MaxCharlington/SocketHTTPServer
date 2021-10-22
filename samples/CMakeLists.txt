# Server/client sample that shows Authentification and database capabilities

add_executable(server_sample1.run server_db_auth_sample.cpp)
target_compile_options(server_sample1.run PRIVATE ${WARNING_FLAGS} ${LANG_STANDARD})
target_include_directories(server_sample1.run PRIVATE ${PROJECT_DIR})
target_include_directories(server_sample1.run PRIVATE ${PROJECT_DIR}/src)

add_executable(client_sample1.run client_db_auth_sample.cpp)
target_compile_options(client_sample1.run PRIVATE ${WARNING_FLAGS} ${LANG_STANDARD})
target_include_directories(client_sample1.run PRIVATE ${PROJECT_DIR})
target_include_directories(client_sample1.run PRIVATE ${PROJECT_DIR}/src)

# Database file installation
file(COPY ${PROJECT_DIR}/src/database/db.txt
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
