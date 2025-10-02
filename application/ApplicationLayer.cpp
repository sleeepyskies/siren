#include "ApplicationLayer.hpp"

#include "core/Application.hpp"
#include "events/WindowEvent.hpp"

namespace siren
{

void ApplicationLayer::onAttach()
{
    const auto& appProps = core::Application::get().getProperties();
    const std::filesystem::path path =
        appProps.workingDirectory / "resources" / "models" / "gltf" / "stick_man" / "scene.gltf";
    modelID = core::Application::get().getAssetManager().loadModelFromRelativePath(path);

    core::renderer::Renderer::init();
}

void ApplicationLayer::onDetach()
{
    core::renderer::Renderer::shutdown();
}

void ApplicationLayer::onUpdate(const float delta)
{
}

void ApplicationLayer::onRender()
{
    core::Application& app = core::Application::get();
    core::renderer::Renderer::beginScene();

    const core::Ref<core::geometry::Model> meshPtr =
        core::Application::get().getAssetManager().getModelByID(modelID);
    for (const auto& submesh : meshPtr->getSubMeshes()) {
        core::renderer::Renderer::submit(submesh.getVertexArray(), submesh.getMaterial(), { 1 });
    }

    core::renderer::Renderer::endScene();
}

void ApplicationLayer::onEvent(core::Event& e)
{
    core::EventHandler handler(e);
    handler.handle<core::WindowResizeEvent>([this](core::WindowResizeEvent& e) {
        return false; // do not consume resize events
    });
}

} // namespace siren
