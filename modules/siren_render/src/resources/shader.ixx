module;

#include <optional>
#include <string>
#include <flat_map>

export module siren.render.shader;

import siren.render.render_resource;
import siren.math;
import siren.render.device;

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
    /// @brief The optional label of the shader.
    std::optional<std::string> label;
    /// @brief The source code of the stage.
    std::string source;
};

/** @brief Describes a @ref Shader to be created. */
export struct ShaderDescriptor {
    /// @brief The source code for each stage of the Shader.
    std::flat_map<ShaderStage, ShaderStageData> source;
    /// @brief The optional label of the shader.
    std::optional<std::string> label;
};

/**
 * @brief Represents a shader compiled on the GPU. Provides an interface to interact
 * with the GPU shader object. However, does not retain any information post compile.
 * To recover information after compiling, see @ref ShaderAsset.
 * @todo THIS USES DIRECT GL CALLS UH OH
 */
class Shader : public RenderResource<Shader> {
    using Base = RenderResource<Shader>;

public:
    Shader(
        Device* device,
        Handle handle
    );
    ~Shader();

    // todo: should we have all these uniforms? maybe set material params via SSBO and use index

    /** @brief Retrieves the uniform location given the uniform name. Returns -1 on no find. */
    auto uniform_location(const std::string& name) const -> i32;

    /** @brief Sets a uniform boolean. Prefer to use bitmasks as this wastes previous memory. */
    auto set_uniform(const std::string& name, bool value) const -> void;
    /** @brief Sets a uniform i32. */
    auto set_uniform(const std::string& name, i32 value) const -> void;
    /** @brief Sets a uniform u32. */
    auto set_uniform(const std::string& name, u32 value) const -> void;
    /** @brief Sets a uniform f32. */
    auto set_uniform(const std::string& name, f32 value) const -> void;
    /** @brief Sets a uniform glm::vec2. */
    auto set_uniform(const std::string& name, glm::vec2 value) const -> void;
    /** @brief Sets a uniform glm::vec3. */
    auto set_uniform(const std::string& name, glm::vec3 value) const -> void;
    /** @brief Sets a uniform glm::vec4. */
    auto set_uniform(const std::string& name, glm::vec4 value) const -> void;
    /** @brief Sets a uniform glm::mat3. */
    auto set_uniform(const std::string& name, const glm::mat3& value) const -> void;
    /** @brief Sets a uniform glm::mat4. */
    auto set_uniform(const std::string& name, const glm::mat4& value) const -> void;
    /** @brief Sets a uniform texture. */
    auto set_uniform_texture(const std::string& name, i32 slot) const -> void;

    /** @brief Returns the descriptor associated with this Shader. */
    [[nodiscard]] auto descriptor() const noexcept -> const ShaderDescriptor&;
};

/**
 * @brief An asset wrapper around a @ref Shader. Provides access to the shader,
 * as well as retains information of the shader.
 * @todo MOVE THIS SOMEWHERE ELSE
 */
struct ShaderAsset : Asset {
    /** @brief The underlying GPU Shader object. */
    Shader shader;
    /** @brief Saved source code for all shader stages. */
    std::flat_map<ShaderStage, std::string> stage_sources;
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

// ========================= UNIFORMS =========================

auto Shader::uniform_location(const std::string& name) const -> i32 {
    const auto it = m_uniform_cache.find(name);
    if (it == m_uniform_cache.end()) {
        Logger::renderer->warn("Could not find uniform location for uniform {}", name);
        return -1;
    }
    return it->second;
}

void Shader::set_uniform(const std::string& name, const bool value) const {
    // we use a 32-bit integer here for a bool, which is by
    // no means efficient. best would be setting up a bit mask
    glProgramUniform1i(m_handle.value, uniform_location(name), value);
}

void Shader::set_uniform(const std::string& name, const i32 value) const {
    glProgramUniform1i(m_handle.value, uniform_location(name), value);
}

void Shader::set_uniform(const std::string& name, const uint32_t value) const {
    glProgramUniform1ui(m_handle.value, uniform_location(name), value);
}

void Shader::set_uniform(const std::string& name, const f32 value) const {
    glProgramUniform1f(m_handle.value, uniform_location(name), value);
}

void Shader::set_uniform(const std::string& name, const glm::vec2 value) const {
    glProgramUniform2f(m_handle.value, uniform_location(name), value.x, value.y);
}

void Shader::set_uniform(const std::string& name, const glm::vec3 value) const {
    glProgramUniform3f(m_handle.value, uniform_location(name), value.x, value.y, value.z);
}

void Shader::set_uniform(const std::string& name, const glm::vec4 value) const {
    glProgramUniform4f(m_handle.value, uniform_location(name), value.x, value.y, value.z, value.w);
}

void Shader::set_uniform(const std::string& name, const glm::mat3& value) const {
    glProgramUniformMatrix3fv(m_handle.value, uniform_location(name), 1, false, glm::value_ptr(value));
}

void Shader::set_uniform(const std::string& name, const glm::mat4& value) const {
    glProgramUniformMatrix4fv(m_handle.value, uniform_location(name), 1, false, glm::value_ptr(value));
}

void Shader::set_uniform_texture(const std::string& name, const i32 slot) const {
    glProgramUniform1i(m_handle.value, uniform_location(name), slot);
}

auto Shader::descriptor() const noexcept -> const ShaderDescriptor& { return m_device->shader_descriptor(m_handle); }

} // namespace siren::render
