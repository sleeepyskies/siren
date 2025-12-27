project(sandbox)

set(SOURCE
        src/main.cpp

        src/SandboxApp.cpp
        src/scripts/ThirdPersonCamera.cpp
        src/scripts/PlayerController.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE})

target_link_libraries(${PROJECT_NAME} core)
target_include_directories(${PROJECT_NAME} PRIVATE src)

# copy assimp to right place
add_custom_command(TARGET sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:assimp>"
        $<TARGET_FILE_DIR:sandbox>
)
