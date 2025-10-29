#include "EditorApp.hpp"
#include "EditorLayer.hpp"
#include "utilities/UUID.hpp"
#include <slog.hpp>

int main(const int argc, char* argv[])
{
    slog::logLevel = slog::Level::DEBUG;
    siren::utilities::UUID::setSeed(1);

    siren::editor::EditorApp::create(
                {
                    .name = "Siren Editor",
                    .renderAPI = siren::core::App::Properties::RenderAPI::OPENGL,
                    .OS = siren::core::App::Properties::OS::WINDOWS
                }
            )
            .run();

    return 0;
}
