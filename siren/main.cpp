#include "TestLayer.hpp"
#include "core/Application.hpp"
#include "core/DebugLayer.hpp"
#include "core/Window.hpp"

// opengl has a right-handed coordinate system, so:
//  +x = right
//  +y = up
//  +z = behind
//
// opengl uses column major ordering for matrices, meaning we store a matrices data with columns
// packed together [col1, col2, col3, col4, ...]

int main(int argc, char* argv[])
{
    // TODO:
    //  - Macro for log level setting based on build type
    //  - Command line argument parsing
    //  - Platform macro detection
    slog::logLevel = slog::Level::INFO;

    siren::core::Window::Properties windowProps{};
    windowProps.backgroundColor = glm::vec4{ 0.12, 0.12, 0.17, 1. };
    windowProps.vSyncEnabled    = true;

    siren::core::Application::Properties appProps{};
    appProps.windowProperties = windowProps;

    siren::core::Application app{ appProps };

    app.pushLayer<siren::core::DebugLayer>();
    app.pushLayer<siren::TestLayer>();
    app.run();
    return 0;
}
