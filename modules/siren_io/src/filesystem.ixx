module;

#include <string>
#include <optional>
#include <vector>
#include <span>
#include <system_error>
#include <filesystem>

export module siren.io.filesystem;

import siren.io.file;

/**
 * @brief Collection of utility methods for interacting with a virtual filesystem.
 *
 */
export namespace siren::io::FileSystem {

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

} // namespace siren::io::Filesystem

namespace siren::io {

struct Mount {
    std::string virt;
    Path pyhs;
};

// use std::vector here since the amount of mounts should be tiny
static std::vector<Mount> s_mounts;

void mount(const std::string& virtual_path, const Path& physical_path) {
    if (virtual_path.empty()) { return; }
    if (FileSystem::exists(physical_path)) {
        s_mounts.push_back(Mount{ .virt = virtual_path, .pyhs = physical_path });
    }
}

void unmount(const std::string& v_key) {
    for (auto it = s_mounts.begin(); it != s_mounts.end(); ++it) {
        if (it->virt == v_key) { s_mounts.erase(it); }
    }
}

std::optional<Path> get_physical_path(const std::string& virtual_path) {
    for (auto it = s_mounts.begin(); it != s_mounts.end(); ++it) {
        if (it->virt == virtual_path) { return it->pyhs; }
    }
    return std::nullopt;
}

std::optional<Path> to_virtual(const Path& path, const std::string_view v_key) {
    if (FileSystem::is_virtual(path)) { return path; }

    return get_physical_path(v_key.data()).transform(
        [&v_key, &path] (const Path& physical_path) {
            std::error_code ec;
            return v_key / std::filesystem::relative(path, physical_path, ec);
        }
    );
}

std::optional<Path> to_physical(const Path& path) {
    if (FileSystem::is_physical(path)) { return path; }

    std::string p_str = path.string();

    return [] (const std::string& p) -> std::optional<Mount> {
        for (const auto& m : s_mounts) {
            if (p.starts_with(m.virt)) { return m; }
        }
        return std::nullopt;
    }(p_str).transform(
        [&p_str] (const Mount& m) {
            const u32 idx = m.virt.length();
            return m.pyhs / p_str.erase(0, idx);
        }
    );
}

bool is_virtual(const Path& path) {
    for (const std::string p_str = path.string(); const auto& m : s_mounts) {
        if (p_str.starts_with(m.virt)) { return true; }
    }
    return false;
}

bool is_physical(const Path& path) { return path.is_absolute(); }

bool exists(const Path& path) {
    return to_physical(path).transform(
        [] (const Path& p) {
            std::error_code ec;
            return std::filesystem::exists(p, ec);
        }
    ).value_or(false);
}

bool is_file(const Path& path) {
    return to_physical(path).transform(
        [] (const Path& p) {
            std::error_code ec;
            return std::filesystem::is_regular_file(p, ec);
        }
    ).value_or(false);
}

bool is_dir(const Path& path) {
    return to_physical(path).transform(
        [] (const Path& p) {
            std::error_code ec;
            return std::filesystem::is_directory(p, ec);
        }
    ).value_or(false);
}

std::optional<u64> get_file_size(const Path& path) {
    return to_physical(path).transform(
        [] (const Path& p) {
            std::error_code ec;
            return std::filesystem::file_size(p, ec);
        }
    );
}

bool read_into(const Path& path, std::span<u8> buffer) {
    return to_physical(path).transform(
        [&buffer] (const auto& p) {
            if (!is_file(p)) { return false; }
            File file{ p, FileOpenMode::Read };
            if (!file.can_read()) { return false; }
            file.read(buffer);
            return true;
        }
    ).value_or(false);
}

std::optional<std::vector<u8>> read_bytes(const Path& path) {
    return get_file_size(path).transform(
        [&path] (const u32 size) {
            std::vector<u8> bytes(size);
            read_into(path, bytes);
            return bytes;
        }
    );
}

std::optional<std::string> read_text(const Path& path) {
    return get_file_size(path).transform(
        [&path] (const u32 size) {
            std::string str;
            str.resize(size);
            read_into(path, std::span{ reinterpret_cast<u8*>(str.data()), size });
            return str;
        }
    );
}

bool write(const Path& path, const std::span<u8> buf) {
    auto file = FileSystem::open(path, FileOpenMode::Write);
    if (!file) { return false; }
    return file->write(buf);
}

bool write(const Path& path, const std::string& str) {
    auto file = FileSystem::open(path, FileOpenMode::Write);
    if (!file) { return false; }
    return file->write(std::span{ reinterpret_cast<const u8*>(str.data()), str.size() });
}

std::optional<File> open(const Path& path, FileOpenMode mode) {
    return to_physical(path).transform(
        [mode] (const auto& p) {
            return std::move(File{ p, mode });
        }
    );
}

} // namespace siren::io
