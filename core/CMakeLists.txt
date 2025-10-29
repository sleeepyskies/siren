cmake_minimum_required(VERSION 3.29)
project(core)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# todo: split each part into its own module

add_library(${PROJECT_NAME} STATIC
        src/core/App.cpp
        src/core/Core.cpp
        src/core/Debug.cpp
        src/core/Layer.cpp
        src/core/Profiler.cpp

        src/window/WindowModule.cpp
        src/time/TimeModule.cpp
        src/input/InputModule.cpp
        src/filesystem/FileSystemModule.cpp

        src/assets/AssetModule.cpp
        src/assets/AssetRegistry.cpp
        src/assets/importers/TextureImporter.cpp
        src/assets/importers/ShaderImporter.cpp
        src/assets/importers/MeshImporter.cpp
        src/assets/importers/ImportContext.cpp
        src/assets/ShaderCache.cpp

        src/renderer/material/Material.cpp
        src/renderer/shaders/Shader.cpp
        src/renderer/buffer/VertexBufferLayout.cpp
        src/renderer/buffer/VertexBuffer.cpp
        src/renderer/buffer/IndexBuffer.cpp
        src/renderer/buffer/UniformBuffer.cpp
        src/renderer/buffer/VertexArray.cpp
        src/renderer/Texture.cpp
        src/renderer/RenderModule.cpp
        src/renderer/FrameBuffer.cpp
        src/renderer/gpuStructs.cpp
        src/renderer/renderConfig.cpp

        src/geometry/Mesh.cpp
        src/geometry/primitive.cpp

        src/ecs/core/Scene.cpp
        src/ecs/core/Component.cpp
        src/ecs/core/EntityManager.cpp
        src/ecs/systems/RenderSystem.cpp
        src/ecs/systems/ScriptSystem.cpp

        src/serialization/SceneSerializer.hpp

        src/utilities/spch.cpp
        src/utilities/sobj.cpp
        src/utilities/stb_image.cpp
        src/utilities/UUID.cpp

        src/platform/windows/WindowsTimer.cpp
        src/platform/windows/WindowsInput.cpp
        src/platform/windows/WindowsWindow.cpp

        external/glad/src/glad.c

        src/ui/ui.cpp
        external/imgui/imgui.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
        external/imgui/backends/imgui_impl_glfw.cpp

)

# make sure we use a cpp compiler not a c compiler for pch
target_precompile_headers(core PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/core/src/utilities/spch.hpp>")

target_include_directories(${PROJECT_NAME}
        PUBLIC src
        PUBLIC external/slog
        PUBLIC external/sobj
        PUBLIC external/stb
        PUBLIC external/nlohmann # TODO: remove this dep, sshg should be yaml based
        PUBLIC external/fkYAML
        PUBLIC external/secs
        PUBLIC external/glad/include
        PUBLIC external/imgui
)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw assimp)
