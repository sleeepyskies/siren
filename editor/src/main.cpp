#include "EditorApp.hpp"
#include <slog.hpp>

int main(const int argc, char* argv[])
{
    slog::logLevel = slog::Level::DEBUG;

    siren::core::App::create().defaultModules().pushLayer<EditorLayer> run();

    return 0;
}
