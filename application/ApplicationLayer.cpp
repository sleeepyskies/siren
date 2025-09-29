#include "ApplicationLayer.hpp"

#include "core/Application.hpp"
#include "events/WindowEvent.hpp"
#include "scene/Scene.hpp"
#include "secs/Scene.hpp"

namespace siren
{

void ApplicationLayer::onAttach()
{
    m_scene = core::makeUref<core::Scene>(core::Application::get().getWindow());

    m_objectShader = core::makeUref<core::renderer::Shader>("../resources/shaders/vertex.vert",
                                                            "../resources/shaders/fragment.frag");
    m_lightShader  = core::makeUref<core::renderer::Shader>("../resources/shaders/vertex.vert",
                                                           "../resources/shaders/light.frag");

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
    m_scene->onUpdate(delta, core::Application::get().getWindow());
    core::secs::Scene s{};
    s.onUpdate(delta);
}

void ApplicationLayer::onRender()
{
    m_scene->draw(*m_objectShader, *m_lightShader);
}

void ApplicationLayer::onEvent(core::Event& e)
{
    core::EventDispatcher dispatcher(e);
    dispatcher.dispatch<core::WindowResizeEvent>([this](core::WindowResizeEvent& e) {
        m_scene->onWindowResize(e.getWidth(), e.getHeight());
        return false; // do not consume resize events
    });
}

} // namespace siren
