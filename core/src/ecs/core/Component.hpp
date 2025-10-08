#pragma once

namespace siren::ecs
{

/// @brief Unique Handle for components. Is not randomly assigned, but sequentially. 0 is an invalid
/// handle.
using ComponentHandle                       = uint32_t;
constexpr ComponentHandle INVALID_COMPONENT = 0;

// ReSharper disable once CppClassCanBeFinal
/**
 * @brief The base Component abstract class that all other Components must implement.
 */
struct Component {
    Component();
    const ComponentHandle handle;

    virtual ~Component()                  = default;
    Component(Component&)                 = delete; // no copying please
    Component operator=(const Component&) = delete;
};

} // namespace siren::ecs
