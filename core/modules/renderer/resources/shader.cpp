#include "shader.hpp"

#include "glm/gtc/type_ptr.hpp"

#include "../../platform/opengl/gl.hpp"
#include "renderer/device.hpp"


namespace siren::core
{
Shader::Shader(
    Device* device,
    const ShaderHandle handle
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
} // namespace siren::core
