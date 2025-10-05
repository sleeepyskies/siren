#include "Debug.hpp"

#include "slog.hpp"

namespace core::debug
{

std::string sourceToString(const GLenum source)
{
    switch (source) {
        case GL_DEBUG_SOURCE_API            : return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM  : return "WINDOW SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY    : return "THIRD PARTY";
        case GL_DEBUG_SOURCE_APPLICATION    : return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER          :
        default                             : return "UNKNOWN";
    }
}

std::string typeToString(const GLenum type)
{
    switch (type) {
        case GL_DEBUG_TYPE_ERROR              : return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR : return "UNDEFINED BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY        : return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE        : return "PERFORMANCE";
        case GL_DEBUG_TYPE_OTHER              : return "OTHER";
        case GL_DEBUG_TYPE_MARKER             : return "MARKER";
        default                               : return "UNKNOWN";
    }
}

std::string severityToString(const GLenum severity)
{
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH        : return "HIGH";
        case GL_DEBUG_SEVERITY_MEDIUM      : return "MEDIUM";
        case GL_DEBUG_SEVERITY_LOW         : return "LOW";
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
        default                            : return "UNKNOWN";
    }
}

void GLFWErrorCallback(int errorCode, const char* description)
{
    err("GLFW Error encountered. Code: {}, description: {}", errorCode, description);
}

void OpenGLErrorCallback(const GLenum source, const GLenum type, const GLuint id,
                         const GLenum severity, const GLsizei length, const GLchar* message,
                         const void* userParam)
{
    // limit to 5 repeats
    static std::unordered_map<uint32_t, int> count{};
    if (++count[id] > 5) { return; }

    // source := where the error message comes from
    std::string sourceString   = sourceToString(source);
    std::string typeString     = typeToString(type);
    std::string severityString = severityToString(severity);

    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        err("OpenGL: [{} - {} ({})]: [{}] {}",
            severityString,
            typeString,
            id,
            sourceString,
            message);
    } else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
        wrn("OpenGL: [{} - {} ({})]: [{}] {}",
            severityString,
            typeString,
            id,
            sourceString,
            message);
    } else if (severity == GL_DEBUG_SEVERITY_LOW) {
        nfo("OpenGL: [{} - {} ({})]: [{}] {}",
            severityString,
            typeString,
            id,
            sourceString,
            message);
    } else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        trc("OpenGL: [{} - {} ({})]: [{}] {}",
            severityString,
            typeString,
            id,
            sourceString,
            message);
    }
}

} // namespace core::debug
