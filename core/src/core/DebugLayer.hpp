#pragma once

#include "Layer.hpp"

namespace siren::core
{

class DebugLayer final : public Layer
{
public:
    DebugLayer()           = default;
    ~DebugLayer() override = default;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(float delta) override;
    void onRender() override;
    void onEvent(events::Event& e) override;

private:
    float m_previousFrameElapsed = 0;
    float m_accumulatedTime      = 0;
};

} // namespace siren::core
