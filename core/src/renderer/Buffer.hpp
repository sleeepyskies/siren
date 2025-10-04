#pragma once

#include "Shader.hpp"
#include "core/GL.hpp"
#include "utilities/spch.hpp"

namespace core::renderer
{

using BufferID = GLuint;

/**
 * @brief Defines the usage of a Buffer.
 */
enum BufferUsage {
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

struct IndexDataType {
    enum IndexTEnum {
        /// @brief Index Unsigned Int
        INDEX_UINT   = GL_UNSIGNED_INT,
        /// @brief Index Unsigned Short
        INDEX_USHORT = GL_UNSIGNED_SHORT,
        /// @brief Index Unsigned Char
        INDEX_UBYTE  = GL_UNSIGNED_BYTE,
    };

    IndexTEnum type;
    /// @brief Returns the size of the index type in bytes
    size_t size() const
    {
        switch (type) {
            case INDEX_UINT  : return 4;
            case INDEX_USHORT: return 2;
            case INDEX_UBYTE : return 1;
        }
        SirenAssert(false, "Encountered invalid IndexDataType");
    }

    explicit IndexDataType(const IndexTEnum type) : type(type)
    {
    }
    explicit IndexDataType(const int type) : type(static_cast<IndexTEnum>(type))
    {
    }
};

struct AttributeDataType {
    enum AttributeTEnum {
        ATTRIBUTE_BYTE   = GL_BYTE,
        ATTRIBUTE_UBYTE  = GL_UNSIGNED_BYTE,
        ATTRIBUTE_SHORT  = GL_SHORT,
        ATTRIBUTE_USHORT = GL_UNSIGNED_SHORT,
        ATTRIBUTE_UINT   = GL_UNSIGNED_INT,
        ATTRIBUTE_FLOAT  = GL_FLOAT,
    };

    AttributeTEnum type;
    int count;

    size_t size() const
    {
        switch (type) {
            case ATTRIBUTE_BYTE  : return 1 * count;
            case ATTRIBUTE_UBYTE : return 1 * count;
            case ATTRIBUTE_SHORT : return 2 * count;
            case ATTRIBUTE_USHORT: return 2 * count;
            case ATTRIBUTE_UINT  : return 4 * count;
            case ATTRIBUTE_FLOAT : return 4 * count;
        }
        SirenAssert(false, "Encountered invalid AttributeDataType");
    }

    GLenum dataType() const
    {
        return type;
    }

    explicit AttributeDataType(const AttributeTEnum type, const int count)
        : type(type), count(count)
    {
    }
    explicit AttributeDataType(const int type, const int count)
        : type(static_cast<AttributeTEnum>(type)), count(count)
    {
    }
};

/**
 * @brief Describes how the GPU should read the data for one attribute from a Vertex Buffer.
 */
struct VertexBufferAttribute {
    /// @brief The name of this attribute
    std::string name;
    /// @brief The index at which this vertex attribute is bound
    uint32_t index;
    /// @brief The number of components per vertex attribute
    int32_t size;
    /// @brief The datatype of this vertex attribute
    GLenum type;
    /// @brief Whether the data is normalized
    bool normalized = false; // hardcoded to false for now as I have no use for
    /// @brief The byte offset between vertex attributes
    size_t stride;
    /// @brief The byte offset of the first vertex attribute into the whole VBO
    size_t offset;

    VertexBufferAttribute(const std::string& name, const int32_t size, const GLenum type)
        : name(name), size(size), type(type)
    {
    }
};

/**
 * @brief Describes the full layout of a single Vertex Buffer
 */
class VertexBufferLayout
{
public:
    void addVertexAttribute(const VertexBufferAttribute& attribute);
    void close();
    std::vector<VertexBufferAttribute> getLayout() const;
    bool hasAttribute(AllowedShaderAttribute attribute) const;

private:
    std::vector<VertexBufferAttribute> m_attributes{};
    bool m_closed = false;
};

/**
 * @brief Vertex Buffer Object. Represents a raw block of GPU memory storing vertex
 * data. This may be positions, colors, normals, texture coordinates etc...
 */
class VertexBuffer
{
public:
    VertexBuffer(const std::vector<Byte>& data, BufferUsage usage,
                 const VertexBufferLayout& layout);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;
    BufferID id() const;
    VertexBufferLayout getLayout() const;

private:
    BufferID m_id = 0;
    VertexBufferLayout m_layout;
};

/**
 * @brief Element Buffer Object. Represents a block of GPU memory storing
 * indices into a Vertex Buffer. These indices indicate the drawing order of vertices and
 * are used to reduce data duplication.
 */
class IndexBuffer
{
public:
    explicit IndexBuffer(IndexDataType type);
    ~IndexBuffer();

    void uploadIndices(const std::vector<Byte>& indices, BufferUsage usage);
    void bind() const;
    void unbind() const;
    IndexDataType getIndexType() const;
    size_t getIndexCount() const;
    BufferID id() const;

private:
    BufferID m_id = 0;
    IndexDataType m_type;
    size_t m_indicesCount;
};

/**
 * @brief Vertex Array Object. Represents a block of GPU memory describing how
 * to read from a Vertex Buffer. This means it describes the layout of the data.
 * Furthermore, a VAO simplifies logic, as once set up and bound to a VBO and
 * EBO, only the VAO must be bound again before each draw call is performed.
 */
class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void linkVertexBuffer(const Ref<VertexBuffer>& VBO);
    void linkIndexBuffer(const Ref<IndexBuffer>& EBO);
    Ref<VertexBuffer> getVertexBuffer() const;
    Ref<IndexBuffer> getIndexBuffer() const;
    void bind() const;
    void unbind() const;
    BufferID id() const;
    bool hasAttribute(AllowedShaderAttribute attribute) const;

private:
    BufferID m_id = 0;

    Ref<VertexBuffer> m_vertexBuffer = nullptr;
    Ref<IndexBuffer> m_indexBuffer   = nullptr;
};

class UniformBuffer
{
public:
    UniformBuffer();
    ~UniformBuffer();

    void bind() const;
    void unbind() const;
    void uploadData(const std::vector<Byte>& data, BufferUsage usage, uint8_t slot) const;
    BufferID id() const;

private:
    BufferID m_id = 0;
};

} // namespace core::renderer
