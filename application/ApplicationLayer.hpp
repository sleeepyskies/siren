#pragma once

#include "core/Layer.hpp"
#include "events/Event.hpp"

namespace siren
{

class ApplicationLayer final : public core::Layer
{
public:
    ApplicationLayer() = default;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(float delta) override;
    void onRender() override;
    void onEvent(core::Event& e) override;

private:
};

} // namespace siren
