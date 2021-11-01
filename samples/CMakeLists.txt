### Server/client sample that shows Authentification and database capabilities ###

add_executable(server_db_auth_sample server_db_auth_sample.cpp)
target_include_directories(server_db_auth_sample PRIVATE ${PROJECT_DIR}/src)
target_include_directories(server_db_auth_sample PRIVATE ${PROJECT_DIR}/src/http)

add_executable(client_db_auth_sample client_db_auth_sample.cpp)
target_include_directories(client_db_auth_sample PRIVATE ${PROJECT_DIR}/src)
target_include_directories(client_db_auth_sample PRIVATE ${PROJECT_DIR}/src/http)

# Database file installation
file(COPY ${PROJECT_DIR}/src/database/db.txt
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})



### Simple server sample ###

add_executable(server_sample server_sample.cpp)
target_include_directories(server_sample PRIVATE ${PROJECT_DIR}/src)
target_include_directories(server_sample PRIVATE ${PROJECT_DIR}/src/http)
