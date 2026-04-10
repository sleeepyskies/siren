cmake_minimum_required(VERSION 3.30)
project(siren_app)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS src
        FILES
        src/app.ixx
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        siren_ecs
        siren_schedule
        siren_reflect
)
