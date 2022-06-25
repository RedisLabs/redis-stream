cmake_minimum_required(VERSION 3.0.0)
project(redis_stream VERSION 0.1.0)

include(CTest)
enable_testing()
add_subdirectory(deps/hiredis)
add_executable(redis_stream main.c producer.c consumer.c stream.c stream.h producer.h consumer.h)
target_link_libraries(redis_stream PUBLIC hiredis)
target_include_directories(redis_stream PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/deps/hiredis"
                          )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
