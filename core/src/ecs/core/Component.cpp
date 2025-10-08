#include "Component.hpp"

namespace siren::ecs
{

static uint32_t currentComponentHandle = 0;

Component::Component() : handle(++currentComponentHandle)
{
}

} // namespace siren::ecs
