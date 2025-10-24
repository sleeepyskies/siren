#include "Component.hpp"

namespace siren::ecs
{

static u32 currentComponentHandle = 0;

Component::Component() : handle(++currentComponentHandle) {}

ComponentHandle Component::getComponentHandle() const
{
    return handle;
}

} // namespace siren::ecs
