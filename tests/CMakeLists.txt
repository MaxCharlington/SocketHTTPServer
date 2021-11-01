# Responce test
add_executable(http_response_test http_response_test.cpp)
target_include_directories(http_response_test PRIVATE ${PROJECT_DIR}/src)
target_include_directories(http_response_test PRIVATE ${PROJECT_DIR}/src/http)

# Request test
add_executable(http_request_test http_request_test.cpp)
target_include_directories(http_request_test PRIVATE ${PROJECT_DIR}/src)
target_include_directories(http_request_test PRIVATE ${PROJECT_DIR}/src/http)
