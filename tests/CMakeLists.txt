# Responce test
add_executable(http_response_test http_response_test.cpp)
target_compile_options(http_response_test PRIVATE ${LANG_STANDARD})
target_include_directories(http_response_test PRIVATE ${PROJECT_DIR}/src)
target_include_directories(http_response_test PRIVATE ${PROJECT_DIR}/src/http_new)
