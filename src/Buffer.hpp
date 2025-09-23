#pragma once

#include "Vertex.hpp"

#include <cassert>
#include <glad/glad.h>
#include <vector>

/**
 * @brief Defines the usage of a Buffer.
 */
enum BufferUsage {
    /// @brief Best used for data that is set once and persistent such as static
    /// meshes.
    STATIC = GL_STATIC_DRAW,
    /// @brief Best used for data is updated often and persistent like dynamic
    /// meshes.
    DYNAMIC = GL_DYNAMIC_DRAW,
    /// @brief Best used for data that is updated often but not persistent like
    /// UI or short term
    /// effects.
    STREAM = GL_STREAM_DRAW,
};

/**
 * @brief Describes how the GPU should read data from the VBO Buffer.
 */
struct VBOLayout {
    /// @brief The index at which this vertex attribute is bound
    uint32_t index;
    /// @brief The number of components per vertex attribute
    int32_t size;
    /// @brief The datatype of this vertex attribute
    GLenum type;
    /// @brief Whether the data is normalized
    bool normalized;
    /// @brief The byte offset between vertex attributes
    size_t stride;
    /// @brief The byte offset of the first vertex attribute into the whole VBO
    size_t offset;
};

/**
 * @brief Vertex Buffer Object. Represents a block of GPU memory storing vertex
 * data. This may be positions, colors, normals, texture coordinates etc...
 */
class VBO
{
public:
    VBO();
    ~VBO();

    void uploadData(const std::vector<Vertex>& data, BufferUsage usage);
    void bind() const;
    void unbind() const;
    GLuint id() const;

private:
    GLuint m_id = 0;
};

/**
 * @brief Element Buffer Object. Represents a block of GPU memory storing
 * indices into a VBO. These indices indicate the drawing order of vertices and
 * are used to reduce data duplication.
 */
class EBO
{
public:
    EBO();
    ~EBO();

    void uploadIndices(const std::vector<uint32_t>& indices, const BufferUsage usage);
    void bind() const;
    void unbind() const;
    GLuint id() const;

private:
    GLuint m_id = 0;
};

/**
 * @brief Vertex Array Object. Represents a block of GPU memory describing how
 * to read from a VBO. This means it describes the layout of the data.
 * Furthermore, a VAO simplifies logic, as once set up and bound to a VBO and
 * EBO, only the VAO must be bound again before each draw call is performed.
 */
class VAO
{
public:
    VAO();
    ~VAO();

    /**
     * @brief Describes the layout of an attribute in a VBO.
     */
    struct AttributeLayout {
        uint32_t index;
        int32_t size;
        GLenum type;
        bool normalized;
        size_t stride;
        uint32_t offset;
    };

    void linkAttribute(const VBO& VBO, const AttributeLayout& layout);
    void bind() const;
    void unbind() const;
    GLuint id() const;

private:
    GLuint m_id = 0;
};
