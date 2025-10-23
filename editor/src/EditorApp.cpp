#include "EditorApp.hpp"

#include "EditorLayer.hpp"
#include "core/App.hpp"

namespace siren::editor
{

EditorApp::EditorApp(const Properties& properties) : App(properties)
{
    // setup editor specifics
    pushLayer<EditorLayer>();
}

} // namespace siren::editor
