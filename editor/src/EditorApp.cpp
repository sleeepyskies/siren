#include "EditorApp.hpp"

#include "EditorLayer.hpp"
#include "core/App.hpp"


namespace siren::editor
{

EditorApp& EditorApp::create(const Properties& properties)
{
    SirenAssert(!s_instance, "Cannot create multiple instances of Application");
    s_instance = new EditorApp(properties);
    SirenAssert(s_instance, "App initialization failed");

    // all required modules should be added here. order is important
    s_instance->init();
    return *static_cast<EditorApp*>(s_instance);
}

void EditorApp::init()
{
    App::init();
    pushLayer<EditorLayer>();
}
} // namespace siren::editor
