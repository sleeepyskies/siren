cmake_minimum_required(VERSION 3.30)
project(siren_window)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
        PRIVATE
        src/window_plugin.cpp
)

target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET modules TYPE CXX_MODULES
        FILES
        src/module.ixx

        src/config.ixx
        src/events.ixx
        src/window.ixx
        src/window_plugin.ixx
        src/systems.ixx
        src/signals.ixx
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        siren_app
        siren_schedule
        siren_sync
        siren_common
        siren_math
        siren_logger
        siren_signal
        glfw
)

