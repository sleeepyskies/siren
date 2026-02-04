#pragma once

#include "utilities/spch.hpp"


namespace siren::core
{
/// @brief std::optionals for opening a file.
enum class FileOpenMode {
    Read,     ///< @brief Simple read rights from an existing file.
    Write,    ///< @brief Overwrite the files data, if it exists.
    Append,   ///< @brief Append to a given existing file.
    ReadWrite ///< @brief Both read and write privileges.
};

/// @brief Simple type alias for std::filesystem::path.
using Path = std::filesystem::path;

/**
 * @brief File abstraction in siren. Note that this is a heavy class due to using std::ifstream.
 */
class File {
public:
    File(const Path& path, FileOpenMode mode);
    ~File();
    File(const File&)            = delete;
    File& operator=(const File&) = delete;
    File(File&& other) noexcept;
    File& operator=(File&& other) noexcept;

    /// @brief Checks if it is possible to read from this file.
    bool can_read() const;
    /// @brief Checks if it is possible to write to this file.
    bool can_write() const;
    /// @brief Returns this files path.
    Path path() const;

    /// @brief Returns this files size.
    std::optional<u32> size() const;

    /// @brief Reads into the given buffer.
    u32 read(std::span<u8> buffer);
    /// @brief Reads the entire file contents and returns it.
    std::optional<std::vector<u8>> read_all();

    /// @brief Writes from the given buffer to the file.
    bool write(std::span<const u8> buffer);

private:
    FileOpenMode m_mode;
    Path m_path;
    std::optional<u32> m_size;
    std::fstream m_stream;
    std::mutex m_mutex;
};


namespace FileSystem
{
/// @brief Mounts a new virtual file system path.
void mount(const std::string& virtual_path, const Path& physical_path);
/// @brief Unmounts a virtual file system path if present.
void unmount(const std::string& v_key);
/// @brief Returns the physical path of a virtual file system key string.
std::optional<Path> get_physical_path(std::string_view v_key);

/// @brief Takes a physical path, and makes it a virtual path of the given v_key, if it exists.
std::optional<Path> to_virtual(const Path& path, std::string_view v_key);
/// @brief Takes a virtual path, and resolves it to a physical path.
std::optional<Path> to_physical(const Path& path);
/// @brief Checks if the given path is virtual.
bool is_virtual(const Path& path);
/// @brief Checks if the given path is physical.
bool is_physical(const Path& path);

/// @brief Checks if a file/directory exists at the given path.
bool exists(const Path& path);

/// @brief Checks if a file exists at the given path.
bool is_file(const Path& path);
/// @brief Checks if a directory exists at the given path.
bool is_dir(const Path& path);

/// @brief Returns the size of the file at the path, if it exists.
std::optional<u64> get_file_size(const Path& path);

/// @brief Takes either a virtual or physical path, and reads max(buffer.size, file.size) bytes into it.
bool read_into(const Path& path, std::span<u8> buffer);
/// @brief Returns a byte vector of the entire file contents, if present.
std::optional<std::vector<u8>> read_bytes(const Path& path);
/// @brief Returns a string of the entire file contents, if present.
std::optional<std::string> read_text(const Path& path);

/// @brief Writes the given data to the file at the path.
bool write(const Path& path, std::span<const u8> buf);
/// @brief Writes the string to the file at the path.
bool write(const Path& path, const std::string& str);

/// @brief Returns a new File object from the given path and IOMode.
std::optional<File> open(const Path& path, FileOpenMode mode);
} // namespace fs
} // namespace siren::core
