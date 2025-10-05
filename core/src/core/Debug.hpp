#pragma once

#include "core/GL.hpp"

namespace siren::core
{

void GLFWErrorCallback(int errorCode, const char* description);

void OpenGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                         const GLchar* message, const void* userParam);

} // namespace siren::core
