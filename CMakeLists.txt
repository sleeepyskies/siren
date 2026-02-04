cmake_minimum_required(VERSION 3.29)
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

include(dependencies.cmake)

add_subdirectory(core)
add_subdirectory(editor)
add_subdirectory(sandbox)

