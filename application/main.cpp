#include <core/Application.hpp>

#include "TestLayer.hpp"
#include "core/DebugLayer.hpp"

// opengl has a right-handed coordinate system, so:
//  +x = right
//  +y = up
//  +z = behind
//
// opengl uses column major ordering for matrices, meaning we store a matrices data with columns
// packed together [col1, col2, col3, col4, ...]

int main()
{
    // TODO: macro build detection for log level
    slog::logLevel = slog::Level::TRACE;

    core::Window::Properties windowProps{};
    windowProps.backgroundColor = glm::vec4{ 0.12, 0.12, 0.17, 1. };
    windowProps.vSyncEnabled    = true;

    core::Application::Properties appProps{};
    appProps.windowProperties = windowProps;

    core::Application app{ appProps };

    app.pushLayer<core::DebugLayer>();
    app.pushLayer<siren::TestLayer>();
    app.run();
}
