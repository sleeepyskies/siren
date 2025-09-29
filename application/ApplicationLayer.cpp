#include "ApplicationLayer.hpp"

#include "core/Application.hpp"
#include "events/WindowEvent.hpp"
#include "scene/Scene.hpp"

namespace siren
{

void ApplicationLayer::onAttach()
{
    m_scene = core::makeUref<core::Scene>(core::Application::get().getWindow());

    m_objectShader = core::makeUref<core::Shaders>("../resources/shaders/vertex.vert",
                                                   "../resources/shaders/fragment.frag");
    m_lightShader  = core::makeUref<core::Shaders>("../resources/shaders/vertex.vert",
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
}

void ApplicationLayer::onRender()
{
    m_scene->draw(*m_objectShader, *m_lightShader);
}

void ApplicationLayer::onEvent(core::Event& e)
{
    if (e.getType() == core::EventType::WindowResize) {
        auto& wre = static_cast<core::WindowResizeEvent&>(e);
        m_scene->onWindowResize(wre.getWidth(), wre.getHeight());
    }
}

} // namespace siren
