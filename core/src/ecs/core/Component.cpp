#include "Component.hpp"

namespace siren::ecs
{

static uint32_t currentComponentHandle = 0;

Component::Component() : handle(++currentComponentHandle)
{
}

ComponentHandle Component::getComponentHandle() const
{
    return handle;
}

} // namespace siren::ecs
