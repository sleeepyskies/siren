#pragma once

#include "SirenAssert.hpp"
#include "Types.hpp"

#include <filesystem>
#include <functional>
#include <vector>

namespace siren
{

/**
 *
 * @brief Appends data onto the end of the buffer from the given source vector
 *
 * @warning This function does not take stride into account. It assumes we copy from a contiguous
 * block of memory. To account for stride, use @ref copyToBufferWithStride
 *
 * @tparam T The underlying type of the source vector. Does not need to be bytes
 * @param buf The buffer to append to
 * @param vec The source data vector
 * @param byteStart The start given as an offset given as bytes
 * @param byteEnd The end given as an offset given as bytes
 */
template <typename T>
void copyToBuffer(std::vector<unsigned char>& buf, const std::vector<T>& vec,
                  const uint32_t byteStart, const uint32_t byteEnd)
{
    const uint32_t vecSizeBytes = vec.size() * sizeof(T);
    SirenAssert(byteStart <= vecSizeBytes && byteEnd <= vecSizeBytes && byteStart < byteEnd,
                "Invalid copyToBuffer() params");
    const auto ptr = reinterpret_cast<const unsigned char*>(vec.data());
    buf.insert(buf.end(), ptr + byteStart, ptr + byteEnd);
}

/**
 * @brief Appends data onto the end of the buffer from the given source vector taking stride into
 * account. If stride is 0, this function will fall back to @ref copyToBuffer
 *
 * @tparam T The underlying type of the source vector. Does not need to be bytes
 * @param buf The buffer to append to
 * @param vec The source data vector
 * @param start The start given as an offset in bytes
 * @param end The end given as an offset in bytes
 * @param strideBytes The stride in bytes
 * @param dataSizeBytes The size of the data types to be copied in bytes
 */
template <typename T>
void copyToBufferWithStride(std::vector<unsigned char>& buf, const std::vector<T>& vec,
                            const uint32_t start, const uint32_t end, const uint32_t strideBytes,
                            const uint32_t dataSizeBytes)
{
    if (strideBytes == 0) { return copyToBuffer(buf, vec, start, end); }

    const uint32_t vecSizeBytes = vec.size() * sizeof(T);

    SirenAssert(start <= vecSizeBytes && end <= vecSizeBytes && start < end,
                "Invalid copyToBufferWithStride() params");

    for (uint32_t i = start; i < end; i += strideBytes) {
        // TODO: avoid so many reinterpret casts
        const auto ptr = reinterpret_cast<const unsigned char*>(vec.data()) + i;
        buf.insert(buf.end(), ptr, ptr + dataSizeBytes);
    }
}

/**
 * @brief Returns the @ref path relative to the @ref rootPath, as well as cleans the path to be
 * lexically consistent.
 *
 * @param path The path to clean and make relative
 * @param dirPath The directory to be relative from
 */
inline std::filesystem::path getRelativePathTo(const std::filesystem::path& path,
                                               const std::filesystem::path& dirPath)
{
    using namespace std::filesystem;

    if (!exists(path) || !exists(dirPath)) {
        wrn("Could not relativize path, as {} or {} does not exist",
            path.string(),
            dirPath.string());
        return {};
    }

    if (!is_regular_file(path)) {
        wrn("Could not relativize path, as {} is not a regular file", path.string());
        return {};
    }
    if (!is_directory(dirPath)) {
        wrn("Could not relativize path, as {} is not a directory", dirPath.string());
        return {};
    }

    std::filesystem::path path_ = path.is_absolute() ? path : dirPath / path;
    path_                       = path_.lexically_relative(dirPath);
    path_                       = path_.lexically_normal();
    return path_;
}

/**
 * @brief Takes some value (must be POD, so normal types, c style arrays, structs. no pointers or
 * anything like that though) and returns it as an array of bytes.
 */
template <typename T>
std::vector<Byte> toBytesPod(const T& value)
{
    const auto begin = reinterpret_cast<const Byte*>(&value);
    return std::vector<Byte>(begin, begin + sizeof(T));
}

} // namespace siren
