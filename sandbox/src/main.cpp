#include "SandboxApp.hpp"
#include "core/App.hpp"
#include "utilities/UUID.hpp"
#include "slog.hpp"

int main(const int argc, char* argv[])
{
    slog::logLevel = slog::Level::TRACE;
    siren::utilities::UUID::setSeed(69420); // should be build mode dependent

    siren::core::App::create<siren::sandbox::SandboxApp>(
                {
                    .name = "Sandbox",
                    .renderAPI = siren::core::App::Properties::RenderAPI::OPENGL,
                    .OS = siren::core::App::Properties::OS::WINDOWS
                }
            )
            .run();

    return 0;
}
