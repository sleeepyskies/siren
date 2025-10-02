cmake_minimum_required(VERSION 3.29)
project(core)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC
        src/core/Application.cpp
        src/core/Debug.cpp
        src/core/Input.cpp
        src/core/Window.cpp
        src/core/Time.cpp

        src/renderer/Buffer.cpp
        src/renderer/Shader.cpp
        src/renderer/Texture.cpp
        src/renderer/Renderer.cpp

        src/geometry/Camera.cpp
        src/geometry/Material.cpp
        src/geometry/Model.cpp
        src/geometry/Mesh.cpp

        src/assets/AssetManager.cpp
        src/assets/AssetRegistry.cpp
        src/assets/Asset.cpp
        src/assets/TextureImporter.cpp
        src/assets/SceneImporter.cpp

        src/utilities/spch.cpp
        src/utilities/sobj.cpp
        src/utilities/stb_image.cpp
        src/utilities/tiny_gltf.cpp
        src/utilities/stb_image_write.cpp

        external/glad/src/glad.c
)

# make sure we use a cpp compiler not a c compiler for pch
target_precompile_headers(core PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/core/src/utilities/spch.hpp>")

target_include_directories(${PROJECT_NAME}
        PUBLIC src
        PUBLIC external/slog
        PUBLIC external/sobj
        PUBLIC external/stb
        PUBLIC external/tinygltf
        PUBLIC external/nlohmann
        PUBLIC external/glad/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw glm)