module;

#include <string>
#include <unordered_map>

export module siren.asset:shader_asset;

import :asset;

import siren.render;

namespace siren::asset {

/**
 * @brief An asset wrapper around a @ref Shader. Provides access to the shader,
 * as well as retains information of the shader.
 */
export struct ShaderAsset : Asset {
    /** @brief The underlying GPU Shader object. */
    render::Shader shader;
    /** @brief Saved source code for all shader stages. */
    std::unordered_map<render::ShaderStage, std::string> stage_sources;
};

} // namespace siren::asset
