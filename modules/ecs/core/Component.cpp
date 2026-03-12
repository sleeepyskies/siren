#include "Component.hpp"

namespace siren::core
{

static u32 currentComponentHandle = 0;

Component::Component() : handle(++currentComponentHandle)
{
}

ComponentHandle Component::component_handle() const
{
    return handle;
}

} // namespace siren::ecs
