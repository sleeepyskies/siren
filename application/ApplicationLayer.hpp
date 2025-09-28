#pragma once

#include "core/Layer.hpp"
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

private:
    core::uref<core::Scene> m_scene;
    core::uref<core::Shaders> m_objectShader;
    core::uref<core::Shaders> m_lightShader;
};

} // namespace siren
