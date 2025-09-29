#pragma once

#include "core/GL.hpp"
#include "utilities/spch.hpp"

namespace core::renderer
{

class Shader
{
public:
    Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    ~Shader();

    [[nodiscard]] GLuint id() const;
    void use() const;

    [[nodiscard]] GLint getUniformLocation(const std::string& name) const;

    void setUniformBool(const std::string& name, bool value) const;
    void setUniformInt(const std::string& name, int value) const;
    void setUniformImage(const std::string& name, int value) const;
    void setUniformFloat(const std::string& name, float value) const;
    void setUniformVec2(const std::string& name, glm::vec2 value) const;
    void setUniformVec3(const std::string& name, glm::vec3 value) const;
    void setUniformVec4(const std::string& name, glm::vec4 value) const;

    void setUniformMat3(const std::string& name, const glm::mat3& value) const;
    void setUniformMat4(const std::string& name, const glm::mat4& value) const;

private:
    GLuint m_id = 0;
};

} // namespace core::renderer
