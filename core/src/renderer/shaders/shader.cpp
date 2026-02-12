#include "shader.hpp"

#include "core/locator.hpp"

#include "glm/gtc/type_ptr.hpp"
#include "platform/GL.hpp"


namespace siren::core
{
Shader::Shader(
) { }

Shader::~Shader() {
    glDeleteProgram(m_handle.value);
}

void Shader::bind() const {
    glUseProgram(m_handle.value);
}

void Shader::compile(const std::string& vertexSource, const std::string& fragmentSource) {
    if (m_handle.value != 0) { glDeleteProgram(m_handle.value); }
    m_uniform_cache.clear();

    const char* vertexShaderSource   = vertexSource.c_str();
    const char* fragmentShaderSource = fragmentSource.c_str();
    const u32 vertexShader           = glCreateShader(GL_VERTEX_SHADER);
    const u32 fragmentShader         = glCreateShader(GL_FRAGMENT_SHADER);

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
        Logger::renderer->warn("Vertex shader compilation failed with error message: {}", errorInfo);
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, errorInfo);
        Logger::renderer->warn("Fragment shader compilation for failed with error message: {}", errorInfo);
    }

    // link shaders to shaderObject
    m_handle.value = glCreateProgram();
    glAttachShader(m_handle.value, vertexShader);
    glAttachShader(m_handle.value, fragmentShader);
    glLinkProgram(m_handle.value);

    glGetProgramiv(m_handle.value, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_handle.value, 512, nullptr, errorInfo);
        Logger::renderer->warn("Shader linking for failed with error message: {}", errorInfo);
    }

    // cleanup unneeded shader ids
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    i32 uniformCount = 0;
    glGetProgramiv(m_handle.value, GL_ACTIVE_UNIFORMS, &uniformCount);

    if (uniformCount != 0) {
        i32 maxNameLength = 0;
        GLsizei length    = 0;
        GLsizei count     = 0;
        GLenum type       = GL_NONE;
        glGetProgramiv(m_handle.value, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
        const auto uniformName = std::make_unique<char[]>(maxNameLength);

        for (i32 i = 0; i < uniformCount; i++) {
            glGetActiveUniform(m_handle.value, i, maxNameLength, &length, &count, &type, uniformName.get());
            const i32 location = glGetUniformLocation(m_handle.value, uniformName.get());
            if (location != -1) {
                m_uniform_cache[std::string(uniformName.get(), length)] = location;
            }
        }
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
} // namespace siren::core
