cmake_minimum_required(VERSION 3.30)
project(siren_schedule)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET modules TYPE CXX_MODULES
        FILES
        src/module.ixx
        src/scheduler.ixx
        src/traits.ixx
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        siren_app
        siren_ecs
        EnTT::EnTT
)
