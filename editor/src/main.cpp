#include "EditorApp.hpp"
#include "core/Window.hpp"
#include <glm/vec4.hpp>
#include <slog.hpp>

struct CommandLineParams {
    int argumentCount;
    char** arguments;
    CommandLineParams(const int argc, char** argv) : argumentCount(argc), arguments(argv)
    {
    }
};

// todo:
//  - Macro for log level setting based on build type
//  - Command line argument parsing
//  - Platform macro detection

int main(const int argc, char* argv[])
{
    slog::logLevel = slog::Level::DEBUG;

    const CommandLineParams commandLineParams{ argc, argv };

    // todo: parsing of command line args here please, maybe Main class?
    siren::core::Window::Properties windowProps{};
    windowProps.backgroundColor = glm::vec4{ 0.12, 0.12, 0.17, 1. };

    const siren::editor::EditorApp::Properties appProps{ "siren", windowProps };

    siren::editor::EditorApp editor{ appProps };
    editor.run();

    return 0;
}
