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
        src/core/DebugLayer.cpp

        src/renderer/Buffer.cpp
        src/renderer/Shader.cpp
        src/renderer/Texture.cpp
        src/renderer/Renderer.cpp
        src/renderer/VertexBufferLayout.cpp
        src/renderer/VertexBuffer.cpp
        src/renderer/IndexBuffer.cpp
        src/renderer/VertexArray.cpp

        src/geometry/Camera.cpp
        src/geometry/Material.cpp
        src/geometry/Model.cpp
        src/geometry/Mesh.cpp

        src/assets/AssetManager.cpp
        src/assets/AssetRegistry.cpp
        src/assets/importers/SceneImporter.cpp
        src/assets/importers/TextureImporter.cpp
        src/assets/importers/ShaderImporter.cpp
        src/assets/importers/ModelImporter.cpp

        src/ecs/core/Scene.cpp
        src/ecs/core/Component.cpp
        src/ecs/core/EntityManager.cpp

        src/ecs/systems/ThirdPersonCameraSystem.cpp
        src/ecs/systems/PlayerControllerSystem.cpp
        src/ecs/systems/RenderSystem.cpp

        src/utilities/spch.cpp
        src/utilities/sobj.cpp
        src/utilities/stb_image.cpp
        src/utilities/UUID.cpp

        external/glad/src/glad.c
)

# make sure we use a cpp compiler not a c compiler for pch
target_precompile_headers(core PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/core/src/utilities/spch.hpp>")

target_include_directories(${PROJECT_NAME}
        PUBLIC src
        PUBLIC external/slog
        PUBLIC external/sobj
        PUBLIC external/stb
        PUBLIC external/nlohmann
        PUBLIC external/secs
        PUBLIC external/glad/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw glm assimp)