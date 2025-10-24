#pragma once

#include "../../platform/GL.hpp"

namespace siren::core
{

// todo: make a base Buffer class with helper methods?
// todo: make renderer stuff api agnostic

/// @brief A buffers unique ID
using BufferID = GLuint;

/**
 * @brief Defines the usage of a Buffer.
 */
enum class BufferUsage {
    /// @brief Best used for data that is set once and persistent such as static
    /// meshes.
    STATIC  = GL_STATIC_DRAW,
    /// @brief Best used for data is updated often and persistent like dynamic
    /// meshes.
    DYNAMIC = GL_DYNAMIC_DRAW,
    /// @brief Best used for data that is updated often but not persistent like
    /// UI or short term effects.
    STREAM  = GL_STREAM_DRAW,
};

} // namespace siren::core
