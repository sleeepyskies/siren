module;

#include <optional>
#include <string>
#include <flat_map>

export module siren.render:shader;

import :render_resource;
import :device;

import siren.log;
import siren.math;

namespace siren::render {

/** @brief Represents the various possible shader stages. */
export enum class ShaderStage {
    /** @brief A Vertex Shader. */
    Vertex,
    /** @brief A Fragment Shader. */
    Fragment,
    /** @brief A Geometry Shader (optional). */
    Geometry,
    /** @brief A Compute Shader. */
    Compute,
    /** @brief A Task Shader (optional). */
    Task,
    /** @brief A Mesh Shader (replaces Vertex + Fragment stages). */
    Mesh,
};

/** @brief Holds information on a single shader stage. */
export struct ShaderStageData {
    /** @brief The optional label of the shader. */
    std::optional<std::string> label;
    /** @brief The source code of the stage. */
    std::string source;
};

/** @brief Describes a @ref Shader to be created. */
export struct ShaderDescriptor {
    /** @brief The source code for each stage of the Shader. */
    std::flat_map<ShaderStage, ShaderStageData> source;
    /** @brief The optional label of the shader. */
    std::optional<std::string> label;
};

/**
 * @brief Represents a shader compiled on the GPU. Provides an interface to interact
 * with the GPU shader object. However, does not retain any information post compile.
 * To recover information after compiling, see @ref ShaderAsset.
 */
export class Shader : public RenderResource<Shader> {
    using Base = RenderResource<Shader>;

public:
    Shader(
        Device* device,
        Handle handle
    );
    ~Shader();

    /** @brief Returns the descriptor associated with this Shader. */
    [[nodiscard]] auto descriptor() const noexcept -> const ShaderDescriptor&;
};

Shader::Shader(
    Device* device,
    const Handle handle
) : Base(device, handle) { }

Shader::~Shader() {
    if (m_device && m_handle.is_valid()) {
        m_device->destroy_shader(m_handle);
    }
}

auto Shader::descriptor() const noexcept -> const ShaderDescriptor& { return m_device->shader_descriptor(m_handle); }

} // namespace siren::render
