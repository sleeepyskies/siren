#include "Shader.hpp"

#include "glm/gtc/type_ptr.hpp"


namespace siren::core
{
Shader::Shader(
    const std::string& name,
    const std::string& vertexSource,
    const std::string& fragmentSource
)
    : Asset(name), m_vertexSource(vertexSource), m_fragmentSource(fragmentSource)
{
    const char* vertexShaderSource   = vertexSource.c_str();
    const char* fragmentShaderSource = fragmentSource.c_str();
    const GLuint vertexShader        = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fragmentShader      = glCreateShader(GL_FRAGMENT_SHADER);

    // set the source data for the shaders
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    GLint success;
    char errorInfo[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, errorInfo);
        err("Vertex shader compilation for {} failed with error message: {}", GetName(), errorInfo);
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, errorInfo);
        err("Fragment shader compilation for {} failed with error message: {}", GetName(), errorInfo);
    }

    // link shaders to shaderObject
    m_id = glCreateProgram();
    glAttachShader(m_id, vertexShader);
    glAttachShader(m_id, fragmentShader);
    glLinkProgram(m_id);

    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_id, 512, nullptr, errorInfo);
        err("Shader linking for {} failed with error message: {}", GetName(), errorInfo);
    }

    // cleanup unneeded shader ids
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    i32 uniformCount = 0;
    glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &uniformCount);

    if (uniformCount != 0) {
        i32 maxNameLength = 0;
        GLsizei length    = 0;
        GLsizei count     = 0;
        GLenum type       = GL_NONE;
        glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
        const auto uniformName = CreateOwn<char[]>(maxNameLength);

        for (i32 i = 0; i < uniformCount; i++) {
            glGetActiveUniform(m_id, i, maxNameLength, &length, &count, &type, uniformName.get());
            const i32 location = glGetUniformLocation(m_id, uniformName.get());
            if (location != -1) {
                m_uniformCache[std::string(uniformName.get(), length)] = location;
            }
        }
    }
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

void Shader::Bind() const
{
    glUseProgram(m_id);
}

// ========================= UNIFORMS =========================

i32 Shader::GetUniformLocation(const std::string& name) const
{
    const auto it = m_uniformCache.find(name);
    // make this run config dependent
    /*
    SirenAssert(
        it != m_uniformCache.end(),
        "Error when retrieving cached uniform location for uniform {} of shader {}.",
        name,
        GetName()
    );
    */
    if (it == m_uniformCache.end()) {
        wrn("Could not find uniform location for uniform {} of shader {}", name, GetName());
        return -1;
    }
    return it->second;
}

void Shader::SetUniform(const std::string& name, const bool value) const
{
    // we use a 32-bit integer here for a bool, which is by
    // no means efficient. best would be setting up a bit mask
    glProgramUniform1i(m_id, GetUniformLocation(name), value);
}

void Shader::SetUniform(const std::string& name, const i32 value) const
{
    glProgramUniform1i(m_id, GetUniformLocation(name), value);
}

void Shader::SetUniform(const std::string& name, const uint32_t value) const
{
    glProgramUniform1ui(m_id, GetUniformLocation(name), value);
}

void Shader::SetUniform(const std::string& name, const float value) const
{
    glProgramUniform1f(m_id, GetUniformLocation(name), value);
}

void Shader::SetUniform(const std::string& name, const glm::vec2 value) const
{
    glProgramUniform2f(m_id, GetUniformLocation(name), value.x, value.y);
}

void Shader::SetUniform(const std::string& name, const glm::vec3 value) const
{
    glProgramUniform3f(m_id, GetUniformLocation(name), value.x, value.y, value.z);
}

void Shader::SetUniform(const std::string& name, const glm::vec4 value) const
{
    glProgramUniform4f(m_id, GetUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::SetUniform(const std::string& name, const glm::mat3& value) const
{
    glProgramUniformMatrix3fv(m_id, GetUniformLocation(name), 1, false, glm::value_ptr(value));
}

void Shader::SetUniform(const std::string& name, const glm::mat4& value) const
{
    glProgramUniformMatrix4fv(m_id, GetUniformLocation(name), 1, false, glm::value_ptr(value));
}

void Shader::SetUniformTexture(const std::string& name, const i32 slot) const
{
    glProgramUniform1i(m_id, GetUniformLocation(name), slot);
}
} // namespace siren::core
