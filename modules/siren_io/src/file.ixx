module;

#include <filesystem>
#include <fstream>
#include <libassert/assert.hpp>

export module siren.io.file;

import siren.common;
import siren.sync;

namespace siren::io {

/**
 * @brief Determines the access rights present when interacting
 * with a file.
 */
export enum class FileOpenMode {
    /** @brief Simple read rights from an existing file. */
    Read,
    /** @brief Overwrite the files data, if it exists. */
    Write,
    /** @brief Append to a given existing file. */
    Append,
    /** @brief Both read and write privileges. */
    ReadWrite
};

/** @brief Simple type alias for std::filesystem::path. */
export using Path = std::filesystem::path;

/**
 * @brief File abstraction in siren.
 *
 * @note This is a heavy class due to using std::ifstream.
 */
export class File {
public:
    File(const Path& path, FileOpenMode mode);
    ~File();
    File(const File&)            = delete;
    File& operator=(const File&) = delete;
    File(File&& other) noexcept;
    File& operator=(File&& other) noexcept;

    /** @brief Checks if it is possible to read from this file. */
    [[nodiscard]] auto can_read() const noexcept -> bool;
    /** @brief Checks if it is possible to write to this file. */
    [[nodiscard]] auto can_write() const noexcept -> bool;
    /** @brief Returns this files path. */
    [[nodiscard]] auto path() const noexcept -> Path;

    /** @brief Returns this files size. */
    [[nodiscard]] auto size() const noexcept -> std::optional<u32>;

    /**
     * @brief Reads into the given buffer.
     * @return The number of bytes read into the buffer.
     */
    [[nodiscard]] auto read(std::span<u8> buffer) -> u32;
    /** @brief Reads the entire file contents and returns it. */
    [[nodiscard]] auto read_all() -> std::optional<std::vector<u8>>;
    /** @brief Reads the entire contents of the file as a string and returns it. */
    [[nodiscard]] auto read_all_text() -> std::optional<std::string>;

    /** @brief Writes from the given buffer to the file. */
    auto write(std::span<const u8> buffer) -> bool;

private:
    FileOpenMode m_mode;
    Path m_path;
    std::optional<u32> m_size;
    sync::Mutex<std::fstream> m_stream;
};

File::File(const Path& path, const FileOpenMode mode) : m_mode(mode), m_size(std::nullopt) {
    auto open_mode = std::ios::binary | std::ios::ate;

    if (mode == FileOpenMode::Read) open_mode |= std::ios::in;
    else if (mode == FileOpenMode::Write) open_mode |= std::ios::out | std::ios::trunc;
    else if (mode == FileOpenMode::Append) open_mode |= std::ios::out | std::ios::app;
    else if (mode == FileOpenMode::ReadWrite) open_mode |= std::ios::in | std::ios::out;

    auto guard = m_stream.lock();
    guard->open(m_path, open_mode);
    if (!guard->is_open()) {
        return;
    }
    m_size = static_cast<u32>(guard->tellg());
    // cache size
    guard->seekg(std::ios::beg);
}

File::~File() {
    auto guard = m_stream.lock();
    if (guard->is_open()) {
        guard->close();
    }
}

File::File(
    File&& other
) noexcept : m_mode(other.m_mode),
             m_path(std::move(other.m_path)),
             m_size(other.m_size),
             m_stream(sync::Mutex(other.m_stream.consume())) { }

File& File::operator=(File&& other) noexcept {
    if (this != &other) {
        {
            auto guard = m_stream.lock();
            if (guard->is_open()) { guard->close(); }
        }

        m_mode = other.m_mode;
        m_path = std::move(other.m_path);
        m_size = other.m_size;
        m_stream.set(std::move(other.m_stream.consume()));

        other.m_size = std::nullopt;
    }

    return *this;
}

bool File::can_read() const {
    return (m_mode == FileOpenMode::Read || m_mode == FileOpenMode::ReadWrite) && m_stream.lock()->is_open() && m_size.
            has_value();
}

bool File::can_write() const {
    return
            (m_mode == FileOpenMode::Write ||
                m_mode == FileOpenMode::ReadWrite)
            && m_stream.lock()->is_open() && m_size.has_value();
}

Path File::path() const { return m_path; }

std::optional<u32> File::size() const { return m_size; }

auto File::read(const std::span<u8> buffer) -> u32 {
    if (!can_read()) { return 0; }

    const u32 bufsize = std::min(size().value(), static_cast<u32>(buffer.size_bytes()));

    auto guard = m_stream.lock();
    guard->read(reinterpret_cast<char*>(buffer.data()), bufsize);
    return guard->gcount();
}

std::optional<std::vector<u8>> File::read_all() {
    if (!can_read()) { return std::nullopt; }
    std::vector<u8> buffer(size().value());
    if (m_size.value() == 0) { return std::move(buffer); }

    m_stream.run_scoped(
        [] (const auto& guard) {
            guard->seekg(std::ios::beg);
        }
    );

    if (read(buffer) > 0) { return std::move(buffer); }
    return std::nullopt;
}

auto File::read_all_text() -> std::optional<std::string> {
    return read_all().transform(
        [] (const std::vector<u8>& buffer) {
            return std::string(buffer.begin(), buffer.end());
        }
    );
}

bool File::write(const std::span<const u8> buffer) {
    if (!can_write()) { return false; }
    ASSERT(m_size.has_value(), "Cannot write to file with non existent size");
    m_size     = m_size.value() + static_cast<u32>(buffer.size_bytes());
    auto guard = m_stream.lock();
    guard->write(reinterpret_cast<const char*>(buffer.data()), buffer.size_bytes());
    return true;
}

} // namespace siren::io
