#pragma once

#include "core/Layer.hpp"

#include <renderer/material/Material.hpp>

namespace siren
{

class DebugLayer final : public core::Layer
{
public:
    DebugLayer();
    ~DebugLayer() override;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(float delta) override;
    void onRender() override;
    void onUiRender() override;
    void onEvent(event::Event& e) override;
    std::string getName() const override;

private:
    float m_previousFrameElapsed               = 0;
    float m_accumulatedTime                    = 0;
    bool m_active                              = false;
    assets::AssetHandle capsuleMeshHandle      = assets::AssetHandle::invalid();
    Ref<renderer::Material> m_selectedMaterial = nullptr;
};

} // namespace siren
