#pragma once

#include "Layer.hpp"

namespace core
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
    void onEvent(Event& e) override;

private:
    float m_previousFrameElapsed = 0;
    float m_acuumulatedTime      = 0;
};

} // namespace core
