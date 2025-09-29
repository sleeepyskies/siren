#include "Shaders.hpp"

#include "glm/gtc/type_ptr.hpp"
#include <fstream>

namespace core::renderer
{

std::string loadFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        err("Could not open file {}", path);
        return {};
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    nfo("Loaded file {}", path);
    return ss.str();
}

Shaders::Shaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    // load shader strings
    const std::string vertexString   = loadFile(vertexShaderPath);
    const std::string fragmentString = loadFile(fragmentShaderPath);
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

Shaders::~Shaders()
{
    glDeleteProgram(m_id);
}

GLuint Shaders::id() const
{
    return m_id;
}

void Shaders::use() const
{
    glUseProgram(m_id);
}

// ========================= UNIFORMS =========================

GLint Shaders::getUniformLocation(const std::string& name) const
{
    return glGetUniformLocation(m_id, name.c_str());
}

void Shaders::setUniformBool(const std::string& name, const bool value) const
{
    // we use a 32-bit integer here for a bool, which is by
    // no means efficient. best would be setting up a bit mask
    // but this is fine for now
    glUniform1i(getUniformLocation(name), value);
}

void Shaders::setUniformInt(const std::string& name, const int value) const
{
    glUniform1i(getUniformLocation(name), value);
}

void Shaders::setUniformImage(const std::string& name, const int value) const
{
    glUniform1i(getUniformLocation(name), value);
}

void Shaders::setUniformFloat(const std::string& name, const float value) const
{
    glUniform1f(getUniformLocation(name), value);
}

void Shaders::setUniformVec2(const std::string& name, const glm::vec2 value) const
{
    glUniform2f(getUniformLocation(name), value.x, value.y);
}

void Shaders::setUniformVec3(const std::string& name, const glm::vec3 value) const
{
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void Shaders::setUniformVec4(const std::string& name, const glm::vec4 value) const
{
    glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shaders::setUniformMat3(const std::string& name, const glm::mat3& value) const
{
    glUniformMatrix3fv(getUniformLocation(name), 1, false, glm::value_ptr(value));
}

void Shaders::setUniformMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(getUniformLocation(name), 1, false, glm::value_ptr(value));
}

} // namespace core::renderer
