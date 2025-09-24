cmake_minimum_required(VERSION 3.29)
project(siren)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG master
)

FetchContent_MakeAvailable(glm)

add_subdirectory(external/glfw-3.4)
add_subdirectory(external/glad)

add_executable(
        ${PROJECT_NAME}
        src/main.cpp
        src/Shaders.cpp
        src/Buffer.cpp
        src/Camera.cpp
        src/Model.cpp
        src/Texture.cpp
        src/Mesh.cpp
        src/Window.cpp
        src/Input.cpp
        src/Material.cpp

        src/define_implementation.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw glm opengl32)
target_include_directories(
        ${PROJECT_NAME} PRIVATE
        external/slog
        external/stb
        external/sobj
        external/json
        external/tinygltf
)
