cmake_minimum_required(VERSION 3.30)
project(siren_common)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
        PRIVATE
        src/assert.cpp
)

target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS src
        FILES
        src/module.ixx
        src/concepts.ixx
        src/hashed_string.ixx
        src/typedefs.ixx
        src/identifier_64.ixx
        src/error.ixx
        src/format.ixx
        src/base.ixx
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        fmt::fmt
        siren_reflect
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)
