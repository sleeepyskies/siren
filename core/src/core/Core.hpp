/**
 * @brief Core.hpp
 * @brief File containing useful accessor functions to core Modules.
 */
#pragma once

namespace siren::core
{

// forward declarations
class App;
class InputModule;
class TimeModule;
class WindowModule;
class AssetModule;
class RenderModule;

/// @brief Returns the @App instance.
App& app();
/// @brief Returns the @WindowModule of the @App instance.
WindowModule& window();
/// @brief Returns the @InputModule of the @App instance.
InputModule& input();
/// @brief Returns the @TimeModule of the @App instance.
TimeModule& time();
/// @brief Returns the @AssetModule of the @App instance.
AssetModule& assets();
/// @brief Returns the @RenderModule of the @App instance.
RenderModule& renderer();

} // namespace siren::core
