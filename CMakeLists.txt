cmake_minimum_required(VERSION 3.29)
project(siren)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(dependencies.cmake)

add_subdirectory(core)
add_subdirectory(editor)
add_subdirectory(demo)

