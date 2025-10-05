#pragma once

#include "core/Layer.hpp"
#include "ecs/Scene.hpp"
#include "events/Event.hpp"
#include "geometry/Camera.hpp"
#include "renderer/Buffer.hpp"

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
    secs::Scene m_scene{ "test" };
    assets::AssetHandle m_shaderHandle = assets::AssetHandle::invalid();

    geometry::Camera m_camera;
    bool m_debugCameraEnabled = false;
};

} // namespace siren
