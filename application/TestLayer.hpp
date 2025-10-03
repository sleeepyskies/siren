#pragma once

#include "assets/AssetManager.hpp"
#include "core/Layer.hpp"
#include "events/Event.hpp"

namespace siren
{

class TestLayer final : public core::Layer
{
public:
    TestLayer() = default;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(float delta) override;
    void onRender() override;
    void onEvent(core::Event& e) override;

private:
    core::assets::AssetHandle m_shaderHandle = core::assets::AssetHandle::invalid();
    core::assets::AssetHandle m_modelHandle  = core::assets::AssetHandle::invalid();
    ;
};

} // namespace siren
