cmake_minimum_required(VERSION 3.29)
project(core)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# todo: split each part into its own module

set(IMGUI_SRC
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
        external/imgui/misc/cpp/imgui_stdlib.cpp
)

add_library(${PROJECT_NAME} STATIC
        src/core/App.cpp
        src/core/Debug.cpp
        src/core/Time.cpp
        src/core/FileSystem.cpp

        src/sync/task_pool.cpp

        src/window/WindowModule.cpp
        src/input/InputModule.cpp

        src/assets/loaders/GltfLoader.cpp
        src/assets/loaders/ImageLoader.cpp
        src/assets/AssetPath.cpp

        src/renderer/shaders/Shader.cpp
        src/renderer/VertexLayout.cpp
        src/renderer/resources/Buffer.cpp
        src/renderer/resources/Texture.cpp
        src/renderer/resources/FrameBuffer.cpp
        src/renderer/resources/GraphicsPipeline.cpp
        src/renderer/Renderer.cpp
        src/renderer/GPULight.cpp
        src/renderer/RenderInfo.cpp
        src/renderer/shaders/ShaderLibrary.cpp
        src/renderer/PBRMaterial.cpp

        src/geometry/Primitive.cpp
        src/geometry/VertexBufferBuilder.cpp

        src/ecs/core/world.cpp
        src/ecs/core/Component.cpp
        src/ecs/core/EntityManager.cpp
        src/ecs/systems/RenderSystem.cpp
        src/ecs/systems/ScriptSystem.cpp

        src/events/EventBus.cpp

        src/serialization/SceneSerializer.hpp

        src/utilities/spch.cpp
        src/utilities/stb_image.cpp
        src/utilities/cgltf.cpp
        src/utilities/UUID.cpp

        src/ui/ui.cpp
        src/ui/fonts/FontAwesome.cpp
        src/ui/fonts/Inter.cpp

        src/platform/windows/WindowsInput.cpp
        src/platform/windows/WindowsWindow.cpp

        external/glad/src/glad.c
        src/assets/loaders/ShaderLoader.cpp
        src/platform/opengl/OpenGLDevice.cpp
        src/sync/render_thread.hpp
        src/sync/condition_variable.hpp
        src/sync/condition_variable.cpp
        src/sync/render_thread.cpp
)

target_sources(core PRIVATE ${IMGUI_SRC})

# make sure we use a cpp compiler not a c compiler for pch
target_precompile_headers(core PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/core/src/utilities/spch.hpp>")

target_include_directories(${PROJECT_NAME}
        PUBLIC src
        PUBLIC external/cgltf
        PUBLIC external/stb
        PUBLIC external/fkYAML
        PUBLIC external/secs
        PUBLIC external/glad/include
        PUBLIC external/imgui
        PUBLIC external/entt
        PUBLIC external/spdlog
)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
