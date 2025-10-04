#pragma once

#include "assets/AssetManager.hpp"
#include "core/Layer.hpp"
#include "events/Event.hpp"
#include "geometry/Camera.hpp"

namespace siren
{

class TestLayer final : public core::Layer
{
public:
    TestLayer();

    void onAttach() override;
    void onDetach() override;
    void onUpdate(float delta) override;
    void onRender() override;
    void onEvent(core::Event& e) override;

private:
    core::assets::AssetHandle m_shaderHandle = core::assets::AssetHandle::invalid();
    core::assets::AssetHandle m_modelHandle  = core::assets::AssetHandle::invalid();
    core::Camera m_camera;
};

} // namespace siren
