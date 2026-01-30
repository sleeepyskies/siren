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
