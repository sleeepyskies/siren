#pragma once

#include "assets/Asset.hpp"
#include "core/Layer.hpp"
#include "ecs/core/Scene.hpp"
#include "events/Event.hpp"

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
    assets::AssetHandle m_shaderHandle = assets::AssetHandle::invalid();
};

} // namespace siren
