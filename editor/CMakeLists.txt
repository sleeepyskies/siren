project(editor)

set(SOURCE
        src/main.cpp

        src/EditorApp.cpp
        src/EditorCamera.cpp
        src/SceneViewRenderer.cpp

        src/panel/SceneViewPanel.cpp
        src/panel/SceneHierarchyPanel.cpp
        src/panel/InspectorPanel.cpp

        src/widget/MainMenuBar.cpp
        src/widget/EditorCameraPropertiesWidget.cpp

        src/ui/ui.cpp
        src/ui/fonts/font_awesome.cpp
        src/ui/fonts/inter.cpp

        # remove once there is serialization...
        src/scripts/ThirdPersonCamera.cpp
        src/scripts/PlayerController.cpp
        src/EditorState.hpp
        src/MetaRegistry.cpp
        src/GuiMeta.hpp
        src/GuiMeta.cpp

)

add_executable(${PROJECT_NAME} ${SOURCE})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE})

target_link_libraries(${PROJECT_NAME} core)
target_include_directories(${PROJECT_NAME} PRIVATE src)
