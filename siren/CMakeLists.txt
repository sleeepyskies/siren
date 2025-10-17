project(siren)

set(SOURCE
        main.cpp
        TestLayer.cpp
        DebugLayer.cpp

        scripts/PlayerController.cpp
        scripts/ThirdPersonCamera.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE})

target_link_libraries(${PROJECT_NAME} core)
target_include_directories(${PROJECT_NAME} PRIVATE siren)

# copy assimp to right place
add_custom_command(TARGET siren POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:assimp>"
        $<TARGET_FILE_DIR:siren>
)
