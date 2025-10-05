#pragma once

#include "events/Event.hpp"

namespace siren::core
{

class Layer
{
public:
    virtual ~Layer() = default;

    virtual void onAttach()                = 0;
    virtual void onDetach()                = 0;
    virtual void onUpdate(float delta)     = 0;
    virtual void onRender()                = 0;
    virtual void onEvent(events::Event& e) = 0;
};

} // namespace siren::core
