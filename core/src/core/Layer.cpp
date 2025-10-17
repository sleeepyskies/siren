#include "Layer.hpp"

namespace siren::core
{

void Layer::setEventCallback(const event::EventCallback& callback)
{
    m_eventCallback = callback;
}

} // namespace siren::core