cmake_minimum_required(VERSION 3.30)
project(common)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC
)

target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET modules TYPE CXX_MODULES
        FILES
        src/common.ixx
        src/concepts.ixx
        src/format.ixx
        src/hash.ixx
        src/hashed_string.ixx
        src/typedefs.ixx
        src/error.ixx

        src/sync/sync.ixx
        src/sync/guard.ixx
        src/sync/mutex.ixx
)

