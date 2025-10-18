#include "EditorApp.hpp"

#include "core/Application.hpp"

namespace siren::editor
{

EditorApp::EditorApp(const Properties& properties) : Application(properties)
{
    // setup editor specifics
    pushLayer<EditorLayer>();
}

} // namespace siren::editor
