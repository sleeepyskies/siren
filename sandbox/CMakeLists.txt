project(sandbox)

set(SOURCE
        src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        siren_log
        siren_window
        siren_input
        siren_asset
        siren_render
        siren_time

        EnTT::EnTT
)

target_include_directories(${PROJECT_NAME} PRIVATE src)
