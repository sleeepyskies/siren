cmake_minimum_required(VERSION 3.30)
project(siren_render)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET modules TYPE CXX_MODULES
        FILES
        src/module.ixx
        src/plugin.ixx

        src/device.ixx
        src/command_executor.ixx
        src/render_command.ixx
        src/render_thread.ixx
        src/renderer.ixx
        src/resource_command.ixx
        src/vertex_layout.ixx
        src/vertex_buffer_builder.ixx

        src/resources/fwd.ixx
        src/resources/render_resource.ixx
        src/resources/buffer.ixx
        src/resources/framebuffer.ixx
        src/resources/graphics_pipeline.ixx
        src/resources/image.ixx
        src/resources/sampler.ixx
        src/resources/shader.ixx
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        siren_common
        siren_app
        siren_log
        siren_math
        siren_sync
        siren_schedule
        siren_window

        glfw
        glad::glad
        libassert::assert
)
