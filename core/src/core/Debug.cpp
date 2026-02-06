#include "Debug.hpp"

// todo: dont like this file

namespace siren::core
{

auto source_to_string(const GLenum source) -> std::string {
    switch (source) {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
        case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER:
        default: return "UNKNOWN";
    }
}

auto type_to_string(const GLenum type) -> std::string {
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
        case GL_DEBUG_TYPE_OTHER: return "OTHER";
        case GL_DEBUG_TYPE_MARKER: return "MARKER";
        default: return "UNKNOWN";
    }
}

auto severity_to_string(const GLenum severity) -> std::string {
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
        case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
        case GL_DEBUG_SEVERITY_LOW: return "LOW";
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
        default: return "UNKNOWN";
    }
}

void glfw_error_callback(i32 errorCode, const char* description) {
    Logger::core->error("GLFW Error encountered. Code: {}, description: {}", errorCode, description);
}

void opengl_error_callback(
    const GLenum source,
    const GLenum type,
    const GLuint id,
    const GLenum severity,
    const GLsizei length,
    const GLchar* message,
    const void* userParam
) {
    // limit to 5 repeats
    static std::unordered_map<u32, u32> count{ };
    if (count[id] > 5) {
        return;
    }
    count[id]++;

    // source := where the error message comes from
    const auto sourceString   = source_to_string(source);
    const auto typeString     = type_to_string(type);
    const auto severityString = severity_to_string(severity);

    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        Logger::core->error(
            "OpenGL: [{} - {} ({})]: [{}] {}",
            severityString,
            typeString,
            id,
            sourceString,
            message
        );
    } else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
        Logger::core->warn(
            "OpenGL: [{} - {} ({})]: [{}] {}",
            severityString,
            typeString,
            id,
            sourceString,
            message
        );
    } else if (severity == GL_DEBUG_SEVERITY_LOW) {
        Logger::core->debug(
            "OpenGL: [{} - {} ({})]: [{}] {}",
            severityString,
            typeString,
            id,
            sourceString,
            message
        );
    } else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        Logger::core->info(
            "OpenGL: [{} - {} ({})]: [{}] {}",
            severityString,
            typeString,
            id,
            sourceString,
            message
        );
    }
}
} // namespace siren::core
