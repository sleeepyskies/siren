#include "ApplicationLayer.hpp"

#include "core/Application.hpp"
#include "events/WindowEvent.hpp"
#include "secs/Scene.hpp"

namespace siren
{

void ApplicationLayer::onAttach()
{
    const std::filesystem::path path = core::Application::get().workingDirectory() / "resources" /
                                       "models" / "gltf" / "stick_man" / "scene.gltf";
    core::Application::get().assetManager().loadModelFromRelativePath(path);

    glEnable(GL_DEPTH_TEST);   // depth testing
    glEnable(GL_STENCIL_TEST); // depth testing
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void ApplicationLayer::onDetach()
{
}

void ApplicationLayer::onUpdate(const float delta)
{
}

void ApplicationLayer::onRender()
{
}

void ApplicationLayer::onEvent(core::Event& e)
{
    core::EventHandler handler(e);
    handler.handle<core::WindowResizeEvent>([this](core::WindowResizeEvent& e) {
        return false; // do not consume resize events
    });
}

} // namespace siren
