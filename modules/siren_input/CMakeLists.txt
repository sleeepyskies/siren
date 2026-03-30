cmake_minimum_required(VERSION 3.30)
project(siren_input)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET modules TYPE CXX_MODULES
        FILES
        src/module.ixx
        src/input_plugin.ixx

        src/input.ixx
        src/input_codes.ixx
        src/mappings.ixx
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        siren_common
        siren_ecs
        siren_log
        siren_math
        siren_app
        siren_window
        glfw
)
