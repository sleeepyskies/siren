#pragma once

#include "Model.hpp"
#include <fstream>
#include <glad/glad.h>
#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <slog.hpp>
#include <string>

class Shaders
{
public:
    Shaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    ~Shaders();

    GLuint id() const; // NOLINT(*-use-nodiscard)
    void use() const;

    void registerModel(const Model& model) const;

    GLint getUniformLocation(const std::string& name) const; // NOLINT(*-use-nodiscard)

    void setUniformFloat(const std::string& name, float value) const;
    void setUniformImage(const std::string& name, int value) const;
    void setUniformVec2(const std::string& name, glm::vec2 value) const;
    void setUniformVec3(const std::string& name, glm::vec3 value) const;
    void setUniformVec4(const std::string& name, glm::vec4 value) const;

    void setUniformMat3(const std::string& name, const glm::mat3& value) const;
    void setUniformMat4(const std::string& name, const glm::mat4& value) const;

private:
    GLuint m_id = 0;
};
