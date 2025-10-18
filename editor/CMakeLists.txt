project(editor)

set(SOURCE
        src/EditorApp.cpp
        src/main.cpp
        src/EditorLayer.cpp
        src/EditorLayer.hpp
)

add_executable(${PROJECT_NAME} ${SOURCE})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE})

target_link_libraries(${PROJECT_NAME} core)
target_include_directories(${PROJECT_NAME} PRIVATE siren)

# copy assimp to right place
add_custom_command(TARGET editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:assimp>"
        $<TARGET_FILE_DIR:editor>
)
