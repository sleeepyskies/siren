cmake_minimum_required(VERSION 3.30)
project(siren_opengl)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET modules TYPE CXX_MODULES
        FILES
        src/module.ixx

        src/device.ixx
        src/mappings.ixx
)

# todo migrate to conan

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        siren_render
        siren_log

        glad::glad
        libassert::assert
)

