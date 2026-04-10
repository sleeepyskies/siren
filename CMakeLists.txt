cmake_minimum_required(VERSION 3.30)
project(siren)

#set(CMAKE_COMPILE_WARNING_AS_ERROR ON) # warnings as errors
#add_compile_options(
#        -Wall -Wextra -Wpedantic
#        -Werror
#        -Wshadow -Wconversion -Wsign-conversion
#        -Wnull-dereference -Wdouble-promotion
#        -Wformat=2
#)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(spdlog REQUIRED)
find_package(EnTT REQUIRED)
find_package(cgltf REQUIRED)
find_package(glm REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(fmt REQUIRED)
find_package(opengl_system REQUIRED)
find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(libassert)

add_subdirectory(modules)

add_subdirectory(editor)
add_subdirectory(sandbox)

