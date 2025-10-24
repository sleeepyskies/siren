#include "Shader.hpp"

#include "glm/gtc/type_ptr.hpp"
#include <fstream>

namespace siren::core
{

Shader::Shader(const std::string& name, const std::string& vertexString,
               const std::string& fragmentString)
    : Asset(name)
{
    // load shader strings
    const char* vertexShaderSource   = vertexString.c_str();
    const char* fragmentShaderSource = fragmentString.c_str();

    // create IDs for our shaders
    const GLuint vertexShader   = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // set the source data for the shaders
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

    // compile shaders
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    GLint success;
    char errorInfo[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, errorInfo);
        err("Vertex shader compilation failed with error message: {}", errorInfo);
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, errorInfo);
        err("Fragment shader compilation failed with error message: {}", errorInfo);
    }

    // link shaders to shaderObject
    m_id = glCreateProgram();
    glAttachShader(m_id, vertexShader);
    glAttachShader(m_id, fragmentShader);
    glLinkProgram(m_id);

    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_id, 512, nullptr, errorInfo);
        err("Shader linking failed with error message: {}", errorInfo);
    }

    // cleanup unneeded shader ids
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

void Shader::bind() const
{
    glUseProgram(m_id);
}

// ========================= UNIFORMS =========================

GLint Shader::getUniformLocation(const std::string& name) const
{
    return glGetUniformLocation(m_id, name.c_str());
}

void Shader::setUniformBool(const std::string& name, const bool value) const
{
    // we use a 32-bit integer here for a bool, which is by
    // no means efficient. best would be setting up a bit mask
    // but this is fine for now
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniformInt(const std::string& name, const int value) const
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniformUnsignedInt(const std::string& name, const uint32_t value) const
{
    glUniform1ui(getUniformLocation(name), value);
}

void Shader::setUniformTexture2D(const std::string& name, const int location) const
{
    glUniform1i(getUniformLocation(name), location);
}

void Shader::setUniformFloat(const std::string& name, const float value) const
{
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniformVec2(const std::string& name, const glm::vec2 value) const
{
    glUniform2f(getUniformLocation(name), value.x, value.y);
}

void Shader::setUniformVec3(const std::string& name, const glm::vec3 value) const
{
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void Shader::setUniformVec4(const std::string& name, const glm::vec4 value) const
{
    glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setUniformMat3(const std::string& name, const glm::mat3& value) const
{
    glUniformMatrix3fv(getUniformLocation(name), 1, false, glm::value_ptr(value));
}

void Shader::setUniformMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(getUniformLocation(name), 1, false, glm::value_ptr(value));
}

} // namespace siren::core
