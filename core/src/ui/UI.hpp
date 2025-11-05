/**
 * @file UI.hpp
 * @brief Collection of functions to manage ImGui. Additionally,
 * holds some useful global data such as core fonts, icons etc...
 */
#pragma once

// forward declarations

struct ImFont;


namespace siren::UI
{

/**
 * @brief Namespace holding all core font types.
 */
namespace font
{
inline ImFont* Regular;
inline ImFont* Bold;
inline ImFont* LargeRegular;
inline ImFont* LargeBold;
// inline ImFont* MonoRegular;
// inline ImFont* MonoBold;
} // namespace siren::UI::font

/**
 * @brief Namespace holding all core icons.
 */
namespace icon
{
inline ImFont* Far;
inline ImFont* Fas;
} // namespace siren::UI::icon

/// @brief Initialises ImGui.
void initialize();
/// @brief Shuts down ImGui.
void shutdown();

/// @brief Begins an ImGui render pass.
void begin();
/// @brief Ends an ImGui render pass.
void end();


namespace detail
{
void loadFonts();
void setupStyle();
}

} // namespace siren::UI
