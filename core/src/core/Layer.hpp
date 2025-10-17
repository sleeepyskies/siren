#pragma once

#include "event/Event.hpp"

namespace siren::core
{

class Layer
{
public:
    virtual ~Layer() = default;

    virtual void onAttach()               = 0;
    virtual void onDetach()               = 0;
    virtual void onUpdate(float delta)    = 0;
    virtual void onRender()               = 0;
    virtual void onUiRender()             = 0;
    virtual void onEvent(event::Event& e) = 0;
    virtual void setEventCallback(const event::EventCallback& callback);
    virtual std::string getName() const = 0;

protected:
    event::EventCallback m_eventCallback = [](const event::Event& e) {};
};

} // namespace siren::core
