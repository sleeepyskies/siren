#pragma once

#include "core/GL.hpp"
#include "utilities/spch.hpp"

#include "assets/Asset.hpp"

namespace siren::renderer
{

/**
 * @brief All attributes that the uber shader may take
 */
enum class AllowedShaderAttribute {
    POSITION,
    COLOR,
    NORMAL,
    UV,
};

/**
 * Currently, siren takes an "über-Shader" approach. This means we have a few amount of shaders,
 * that can handle a large amount of cases. This does mean that shader files are larger and
 * complexer, and most likely contain paths that are not use most of the time. However, this
 * approach is the simplest to implement. In the future, a shader variant system would be nice. This
 * would tie shaders much closer to materials as materials would then pick the exact shader they
 * need.
 */
class Shader final : public assets::Asset
{
public:
    Shader(const std::string& name, const std::string& vertexString,
           const std::string& fragmentString);
    ~Shader() override;

    ASSET_TYPE(assets::AssetType::SHADER);

    void bind() const;

    [[nodiscard]] GLint getUniformLocation(const std::string& name) const;

    void setUniformBool(const std::string& name, bool value) const;
    void setUniformInt(const std::string& name, int value) const;
    void setUniformUnsignedInt(const std::string& name, uint32_t value) const;
    void setUniformImage(const std::string& name, int value) const;
    void setUniformFloat(const std::string& name, float value) const;
    void setUniformVec2(const std::string& name, glm::vec2 value) const;
    void setUniformVec3(const std::string& name, glm::vec3 value) const;
    void setUniformVec4(const std::string& name, glm::vec4 value) const;
    void setUniformMat3(const std::string& name, const glm::mat3& value) const;
    void setUniformMat4(const std::string& name, const glm::mat4& value) const;

    void bindUniformBuffer(int slot, GLint bufferID) const;

private:
    GLuint m_id = 0;
};

} // namespace siren::renderer
