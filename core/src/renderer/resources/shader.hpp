#pragma once

#include <flat_map> // todo: why doesnt it find this in spch
#include "utilities/spch.hpp"
#include "renderer/render_resource.hpp"
#include "assets/asset.hpp"


namespace siren::core
{

class Shader;
/// @brief A handle to a @ref Shader.
using ShaderHandle = RenderResourceID<Shader>;

/// @brief Represents the various possible shader stages.
enum class ShaderStage {
    Vertex,   ///< @brief A Vertex Shader.
    Fragment, ///< @brief A Fragment Shader.
    Geometry, ///< @brief A Geometry Shader (optional).
    Compute,  ///< @brief A Compute Shader.
    Task,     ///< @brief A Task Shader (optional).
    Mesh,     ///< @brief A Mesh Shader (replaces Vertex + Fragment stages).
};

/**
 * @brief Holds information on a single shader stage.
 */
struct ShaderStageData {
    /// @brief The optional label of the shader.
    std::optional<std::string> label;
    /// @brief The source code of the stage.
    std::string source;
};

/**
 * @brief Describes a @ref Shader to be created.
 */
struct ShaderDescriptor {
    /// @brief The source code for each stage of the Shader.
    std::flat_map<ShaderStage, ShaderStageData> source;
    /// @brief The optional label of the shader.
    std::optional<std::string> label;
};

/**
 * @brief Represents a shader compiled on the GPU. Provides an interface to interact
 * with the GPU shader object. However, does not retain any information post compile.
 * To recover information after compiling, see @ref ShaderAsset.
 */
class Shader : public RenderResource<Shader> {
    using Base = RenderResource<Shader>;

public:
    Shader(
        Device* device,
        ShaderHandle handle,
        const ShaderDescriptor& descriptor
    );
    ~Shader();

    /// @brief Compiles and creates a new shader object.
    auto compile(const std::string& vertexSource, const std::string& fragmentSource) -> void;

    // todo: should we have all these uniforms? maybe set material params via SSBO and use index

    /// @brief Retrieves the uniform location given the uniform name. Returns -1 on no find.
    auto uniform_location(const std::string& name) const -> i32;

    /// @brief Sets a uniform boolean. Prefer to use bitmasks as this wastes previous memory.
    auto set_uniform(const std::string& name, bool value) const -> void;
    /// @brief Sets a uniform i32.
    auto set_uniform(const std::string& name, i32 value) const -> void;
    /// @brief Sets a uniform u32.
    auto set_uniform(const std::string& name, u32 value) const -> void;
    /// @brief Sets a uniform f32.
    auto set_uniform(const std::string& name, f32 value) const -> void;
    /// @brief Sets a uniform glm::vec2.
    auto set_uniform(const std::string& name, glm::vec2 value) const -> void;
    /// @brief Sets a uniform glm::vec3.
    auto set_uniform(const std::string& name, glm::vec3 value) const -> void;
    /// @brief Sets a uniform glm::vec4.
    auto set_uniform(const std::string& name, glm::vec4 value) const -> void;
    /// @brief Sets a uniform glm::mat3.
    auto set_uniform(const std::string& name, const glm::mat3& value) const -> void;
    /// @brief Sets a uniform glm::mat4.
    auto set_uniform(const std::string& name, const glm::mat4& value) const -> void;
    /// @brief Sets a uniform texture.
    auto set_uniform_texture(const std::string& name, i32 slot) const -> void;

private:
    ShaderDescriptor m_descriptor;
    std::flat_map<std::string, i32> m_uniform_cache{ };
};

struct Define {
    /// @brief The name of the Define aka the #define name value
    std::string name;

    /// @brief The value of the Define aka the #define name value
    union {
        i32 ivalue;
        u32 uvalue;
        bool bvalue;
    } value;
};

/**
 * @brief An asset wrapper around a @ref Shader. Provides access to the shader,
 * as well as retains information of the shader.
 */
struct ShaderAsset : Asset {
    Shader shader;                                         ///< @brief The underlying GPU Shader object.
    std::flat_map<ShaderStage, std::string> stage_sources; ///< @brief Saved source code for all shader stages.
    std::vector<Define> defines;                           ///< @brief All defines of the shader.
};

} // namespace siren::core
