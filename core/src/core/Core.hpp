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
class FileSystemModule;
class WindowModule;
class AssetModule;
class RenderModule;

/// @brief Returns the @App instance.
App& app();
/// @brief Returns the @FileSystemModule of the @App instance.
FileSystemModule& filesystem();
/// @brief Returns the @WindowModule of the @App instance.
WindowModule& window();
/// @brief Returns the @InputModule of the @App instance.
InputModule& input();
/// @brief Returns the @AssetModule of the @App instance.
AssetModule& Assets();
/// @brief Returns the @RenderModule of the @App instance.
RenderModule& Renderer();

} // namespace siren::core
