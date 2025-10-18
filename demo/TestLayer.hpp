#pragma once

#include "assets/Asset.hpp"
#include "core/Layer.hpp"
#include "ecs/core/Scene.hpp"
#include "event/Event.hpp"

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
    void onUiRender() override;
    void onEvent(event::Event& e) override;
    std::string getName() const override;

private:
    ecs::Scene m_scene{};
    assets::AssetHandle m_shaderHandle = assets::AssetHandle::invalid();
};

} // namespace siren
