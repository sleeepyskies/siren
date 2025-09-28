project(application)

set(SOURCE
        main.cpp
        ApplicationLayer.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE})

target_link_libraries(${PROJECT_NAME} core)
target_include_directories(${PROJECT_NAME} PRIVATE application)