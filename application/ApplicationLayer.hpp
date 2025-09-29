#pragma once

#include "core/Layer.hpp"
#include "events/Event.hpp"
#include "scene/Scene.hpp"

namespace siren
{

class ApplicationLayer final : public core::Layer
{
public:
    ApplicationLayer() = default;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(float delta) override;
    void onRender() override;
    void onEvent(core::Event& e) override;

private:
    core::uref<core::Scene> m_scene;
    core::uref<core::renderer::Shader> m_objectShader;
    core::uref<core::renderer::Shader> m_lightShader;
};

} // namespace siren
