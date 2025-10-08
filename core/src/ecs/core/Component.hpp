#pragma once

namespace siren::ecs
{

/// @brief Unique ID for components. Is not randomly assigned, but sequentially.
using ComponentID = uint32_t;

// ReSharper disable once CppClassCanBeFinal
/**
 * @brief The base Component abstract class that all other Components must implement.
 */
struct Component {
    Component();
    const ComponentID id;

    virtual ~Component()                  = default;
    Component(Component&)                 = delete; // no copying please
    Component operator=(const Component&) = delete;
};

} // namespace siren::ecs
