cmake_minimum_required(VERSION 3.30)
project(siren_asset)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
        PUBLIC FILE_SET modules TYPE CXX_MODULES BASE_DIRS src
        FILES
        src/module.ixx

        src/asset.ixx
        src/asset_id.ixx
        src/asset_loader.ixx
        src/asset_config.ixx
        src/asset_handle.ixx
        src/asset_plugin.ixx
        src/asset_server.ixx
        src/asset_path.ixx
        src/asset_pool.ixx
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        siren_app
        siren_ecs
        siren_common
        siren_sync
        siren_logger
)
