cmake_minimum_required(VERSION 3.30)
project(siren_render)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET modules TYPE CXX_MODULES
        FILES
        src/module.ixx
        src/render_plugin.ixx

        src/command_executor.ixx
        src/device.ixx

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
        siren_logger
        siren_math
        siren_sync
        siren_schedule
)
