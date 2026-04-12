cmake_minimum_required(VERSION 3.30)
project(siren_sync)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET modules TYPE CXX_MODULES
        FILES
        src/module.ixx

        src/guard.ixx
        src/mutex.ixx
        src/rw_lock.ixx
        src/condition_variable.ixx
        src/thread_pool.ixx
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        siren_common
        siren_log
)

