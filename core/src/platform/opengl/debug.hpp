#pragma once

#include "../../core/core.hpp"
#include "platform/GL.hpp"


namespace siren::core
{
void glfw_error_callback(i32 errorCode, const char* description);

void opengl_error_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
);
} // namespace siren::core
