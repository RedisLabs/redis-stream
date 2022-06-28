cmake_minimum_required(VERSION 3.0.0)
project(redis-stream VERSION 0.1.0)

include(FetchContent)
FetchContent_Declare(
    hiredis
    GIT_REPOSITORY "https://github.com/redis/hiredis.git"
    GIT_TAG b731283245f3183af527237166261ad0768ba7d4
)
FetchContent_MakeAvailable(hiredis)
include(CTest)
enable_testing()
add_executable(redis-stream main.c producer.c consumer.c stream.c stream.h producer.h consumer.h)
target_link_libraries(redis-stream PUBLIC hiredis)
target_include_directories(redis-stream PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}"
                          )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
