/**
 * @file Shader.hpp
 */
#pragma once

#include "core/Core.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
/**
 * Currently, siren takes an "über-Shader" approach. This means we have a few amount of shaders,
 * that can handle a large amount of cases. This does mean that shader files are larger and
 * complexer, and most likely contain paths that are not use most of the time. However, this
 * approach is the simplest to implement. In the future, a shader variant system would be nice. This
 * would tie shaders much closer to materials as materials would then pick the exact shader they
 * need.
 * @todo Make API agnostic
 */
class Shader final
{
public:
    Shader(
        const std::string& debugName,
        const std::string& vertexSource,
        const std::string& fragmentSource
    );
    ~Shader();

    void Bind() const;
    void Recompile(const std::string& vertexSource, const std::string& fragmentSource);

    i32 GetUniformLocation(const std::string& name) const;

    void SetUniform(const std::string& name, bool value) const;
    void SetUniform(const std::string& name, i32 value) const;
    void SetUniform(const std::string& name, u32 value) const;
    void SetUniform(const std::string& name, float value) const;
    void SetUniform(const std::string& name, glm::vec2 value) const;
    void SetUniform(const std::string& name, glm::vec3 value) const;
    void SetUniform(const std::string& name, glm::vec4 value) const;
    void SetUniform(const std::string& name, const glm::mat3& value) const;
    void SetUniform(const std::string& name, const glm::mat4& value) const;
    void SetUniformTexture(const std::string& name, i32 slot) const;

private:
    std::string m_debugName;
    HashMap<std::string, i32> m_uniformCache{ }; ///< Cached map of uniform names to avoid string parsing

    u32 m_id = 0;
};

class ShaderAsset final // : public Asset
{
    // todo: make this lol
public:
    // ASSET_TYPE(AssetType::Shader);
};
} // namespace siren::core
