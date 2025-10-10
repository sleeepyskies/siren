#pragma once

#include "core/Layer.hpp"
#include "ecs/core/Scene.hpp"
#include "events/Event.hpp"
#include "geometry/Camera.hpp"
#include "renderer/Buffer.hpp"
#include "renderer/SceneRenderer.hpp"

namespace siren
{

class TestLayer final : public core::Layer
{
public:
    TestLayer();
    ~TestLayer() override = default;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(float delta) override;
    void onRender() override;
    void onEvent(events::Event& e) override;

private:
    ecs::Scene m_scene{};
    renderer::SceneRenderer m_sceneRenderer{};
    assets::AssetHandle m_shaderHandle = assets::AssetHandle::invalid();

    ecs::EntityHandle m_cameraEntity = m_scene.create();
};

} // namespace siren
