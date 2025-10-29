/**
 * @file Module.hpp
 */
#pragma once

namespace siren::core
{

/**
 * @brief A Module in Siren provides some service throughout the engine.
 *
 * Modules may be a rendering service, asset management, input handling etc. Users may also create
 * custom modules by inheriting from this class.
 */
class Module
{
public:
    virtual ~Module() = default;

    /// @brief Initializes the module.
    virtual bool initialize() = 0;
    /// @brief Shuts down the module.
    virtual void shutdown() = 0;
    /// @brief Returns this modules name. Mainly used for debugging purposes.
    virtual const char* getName() = 0;
};

} // namespace siren::core
