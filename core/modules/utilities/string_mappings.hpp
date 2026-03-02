#pragma once

#include "renderer/resources/shader.hpp"


namespace siren::utilities
{

/// @brief Maps a @ref ShaderStage to a string.
[[nodiscard]]
constexpr auto to_string(const core::ShaderStage shader_stage) -> std::string_view {
    switch (shader_stage) {
        case core::ShaderStage::Vertex: return "Vertex";
        case core::ShaderStage::Fragment: return "Fragment";
        case core::ShaderStage::Geometry: return "Geometry";
        case core::ShaderStage::Compute: return "Compute";
        case core::ShaderStage::Task: return "Task";
        case core::ShaderStage::Mesh: return "Mesh";
        default: SIREN_ASSERT(false, "Unsupported Shader stage for OpenGL Backend");
    }
}

/// @brief Maps a @ref Code to a string.
[[nodiscard]] constexpr auto to_string(const core::Code code) -> std::string_view {
    switch (code) {
        case core::Code::None: return "None";

        case core::Code::ResourceLocked: return "ResourceLocked";
        case core::Code::IOFail: return "IOFail";

        case core::Code::AssetNotFound: return "AssetNotFound";
        case core::Code::AssetCorrupted: return "AssetCorrupted";
        case core::Code::AssetUnsupported: return "AssetUnsupported";

        case core::Code::DeviceNotPresent: return "DeviceNotPresent";

        default: return "UNKNOWN";
    }
}

} // namespace siren::utilities
