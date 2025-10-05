#pragma once

#include "assets/AssetManager.hpp"
#include "core/Layer.hpp"
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
    assets::AssetHandle m_shaderHandle = assets::AssetHandle::invalid();
    assets::AssetHandle m_modelHandle  = assets::AssetHandle::invalid();
    geometry::Camera m_camera;
    Ref<renderer::UniformBuffer> m_pointLights = makeRef<renderer::UniformBuffer>();
};

} // namespace siren
