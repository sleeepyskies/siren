#pragma once

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "utilities/spch.hpp"


namespace ImGuiSiren
{

/**
 * @brief Scoped ImGui font. Using avoids having to manually call ImGui::PopFont.
 */
struct ScopedFont
{
    explicit ScopedFont(ImFont* font) { ImGui::PushFont(font); }

    ~ScopedFont() { ImGui::PopFont(); }
};

/**
 * @brief Scoped ImGui color alteration.
 */
struct ScopedStyleColor
{
    ScopedStyleColor(const ImGuiCol colIdx, const ImVec4 color) { ImGui::PushStyleColor(colIdx, color); }

    ~ScopedStyleColor() { ImGui::PopStyleColor(); }
};

/**
 * @brief Scoped ImGui style alteration.
 */
struct ScopedStyleVar
{
    ScopedStyleVar(const ImGuiStyleVar style, const ImVec2 value) { ImGui::PushStyleVar(style, value); }

    ScopedStyleVar(const ImGuiStyleVar style, const float value) { ImGui::PushStyleVar(style, value); }

    ~ScopedStyleVar() { ImGui::PopStyleVar(); }
};

/**
 * @brief Scoped ImGui style alteration of x param.
 */
struct ScopedStyleVarX
{
    ScopedStyleVarX(const ImGuiStyleVar style, const float value) { ImGui::PushStyleVarX(style, value); }

    ~ScopedStyleVarX() { ImGui::PopStyleVar(); }
};

/**
 * @brief Scoped ImGui style alteration of x param.
 */
struct ScopedStyleVarY
{
    ScopedStyleVarY(const ImGuiStyleVar style, const float value) { ImGui::PushStyleVarY(style, value); }

    ~ScopedStyleVarY() { ImGui::PopStyleVar(); }
};

/**
 * @brief Helper method used for drawing an icon.
 * @param iconFont To icon font.
 * @param icon The icon to draw.
 */
inline void Icon(ImFont* iconFont, const char* icon)
{
    ScopedFont fas(iconFont);
    ImGui::TextUnformatted(icon);
}

/**
 * @brief Helper method used for drawing an icon on the same line, aka inline.
 * @param iconFont To icon font.
 * @param icon The icon to draw.
 */
inline void InlineIcon(ImFont* iconFont, const char* icon)
{
    ImGui::SameLine();
    ScopedFont fas(iconFont);
    ImGui::TextUnformatted(icon);
}

template <typename... Args>
void Text(const std::string& fmt, Args&&... args)
{
    ImGui::TextUnformatted(std::vformat(fmt, std::make_format_args(args...)).c_str());
}

inline bool SliderUInt(const char* label, siren::u32& uint, const siren::u32 min, const siren::u32 max)
{
    siren::i32 signedInt = static_cast<siren::i32>(uint);
    if (ImGui::SliderInt(label, &signedInt, static_cast<siren::i32>(min), static_cast<siren::i32>(max))) {
        uint = signedInt;
        return true;
    }
    return false;
}


}
