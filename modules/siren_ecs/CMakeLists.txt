cmake_minimum_required(VERSION 3.30)
project(siren_ecs)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET modules TYPE CXX_MODULES
        FILES
        src/module.ixx

        src/world.ixx
        src/system.ixx
        src/signal.ixx
        src/event.ixx
        src/entity.ixx
        src/scheduler.ixx
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        siren_common
        siren_log
        siren_sync
        siren_reflect

        EnTT::EnTT
        libassert::assert
)
