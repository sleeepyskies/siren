#include <core/Application.hpp>

#include "TestLayer.hpp"

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

    core::Application app{ {} };
    const siren::TestLayer layer{};
    app.pushLayer<siren::TestLayer>();
    app.run();
}
