#include "Component.hpp"

namespace siren::ecs
{

static uint32_t currentComponentId = 0;

Component::Component() : id(++currentComponentId)
{
}

} // namespace siren::ecs