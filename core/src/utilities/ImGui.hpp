#pragma once

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>


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

}
