#pragma once

#include "SirenAssert.hpp"
#include <functional>
#include <vector>

namespace core
{

template <typename T> void forEach(const std::vector<T>& vec, const std::function<void(T)>& f)
{
    for (auto& elem : vec) {
        f(elem);
    }
}

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
 * @param start The start given as an offset in bytes
 * @param end The end given as an offset in bytes
 */
template <typename T>
void copyToBuffer(std::vector<unsigned char>& buf, const std::vector<T>& vec, const uint32_t start,
                  const uint32_t end)
{
    SirenAssert(start <= vec.size() && end <= vec.size() && start < end,
                "Invalid copyToBuffer() params (start: {}, end: {}, vec.size() {})",
                start,
                end,
                vec.size());
    const auto ptr = reinterpret_cast<const unsigned char*>(vec.data());
    buf.insert(buf.end(), ptr + start * sizeof(T), ptr + end * sizeof(T));
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

    SirenAssert(start <= vec.size() && end <= vec.size() && start < end,
                "Invalid copyToBufferWithStride() params (start: {}, end: {}, vec.size() {})",
                start,
                end,
                vec.size());

    for (uint32_t i = start; i < end; i += strideBytes) {
        const auto ptr = reinterpret_cast<const unsigned char*>(vec.data()) + i;
        buf.insert(buf.end(), ptr, ptr + dataSizeBytes);
    }
}

} // namespace core
