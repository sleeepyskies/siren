#include "FileSystem.hpp"

#include <fstream>
#include <future>

#include "Core.hpp"

// todo:
//  this could all be made faster without using ifstream, but
//  since i dont care and its not bottle neck for me whatever

namespace siren::core
{
File::File(const Path& path, const FileOpenMode mode) : m_mode(mode), m_size(None) {
    auto open_mode = std::ios::binary | std::ios::ate;

    if (mode == FileOpenMode::Read) open_mode |= std::ios::in;
    else if (mode == FileOpenMode::Write) open_mode |= std::ios::out | std::ios::trunc;
    else if (mode == FileOpenMode::Append) open_mode |= std::ios::out | std::ios::app;
    else if (mode == FileOpenMode::ReadWrite) open_mode |= std::ios::in | std::ios::out;

    m_stream.open(m_path, open_mode);

    if (!m_stream.is_open()) { return; }

    // cache size
    m_size = Some(static_cast<u32>(m_stream.tellg()));
    m_stream.seekg(std::ios::beg);
}

File::~File() { if (m_stream.is_open()) { m_stream.close(); } }

File::File(File&& other) noexcept : m_mode(other.m_mode), m_path(std::move(other.m_path)), m_size(other.m_size),
                                    m_stream(std::move(other.m_stream)) { }

File& File::operator=(File&& other) noexcept {
    if (this != &other) {
        if (m_stream.is_open()) { m_stream.close(); }
        m_mode   = other.m_mode;
        m_path   = std::move(other.m_path);
        m_size   = other.m_size;
        m_stream = std::move(other.m_stream);

        other.m_size = None;
    }

    return *this;
}

bool File::can_read() const {
    return (m_mode == FileOpenMode::Read || m_mode == FileOpenMode::ReadWrite) && m_stream.is_open() && m_size.
            is_some();
}

bool File::can_write() const {
    return (m_mode == FileOpenMode::Write || m_mode == FileOpenMode::ReadWrite) && m_stream.is_open() && m_size.
            is_some();
}

Path File::path() const { return m_path; }

Option<u32> File::size() const { return m_size; }

u32 File::read(const std::span<u8> buffer) {
    if (!can_read()) { return 0; }

    const u32 bufsize = std::min(size().unwrap(), static_cast<u32>(buffer.size_bytes()));

    std::lock_guard lock(m_mutex);
    m_stream.read(reinterpret_cast<char*>(buffer.data()), bufsize);
    return m_stream.gcount();
}

Option<Vector<u8>> File::read_all() {
    if (!can_read()) { return None; }
    Vector<u8> buffer(size().unwrap());
    if (m_size.unwrap() == 0) { return Some(std::move(buffer)); }

    {
        std::lock_guard lock(m_mutex);
        m_stream.seekg(std::ios::beg);
    }

    if (read(buffer)) { return Some(std::move(buffer)); }
    return None;
}

bool File::write(const std::span<const u8> buffer) {
    if (!can_write()) { return false; }
    std::lock_guard lock(m_mutex);
    SIREN_ASSERT(m_size.is_some(), "Cannot write to file with non existent size");
    m_size = Some(m_size.unwrap() + static_cast<u32>(buffer.size_bytes()));
    m_stream.write(reinterpret_cast<const char*>(buffer.data()), buffer.size_bytes());
    return true;
}
} // namespace siren::core

namespace siren::core::FileSystem
{
struct Mount
{
    String virt;
    Path pyhs;
};

// use Vector here since the amount of mounts should be tiny
static Vector<Mount> s_mounts;

void mount(const String& virtual_path, const Path& physical_path) {
    if (virtual_path.empty()) { return; }
    if (FileSystem::exists(physical_path)) {
        s_mounts.push_back(Mount{ .virt = virtual_path, .pyhs = physical_path });
    }
}

void unmount(const String& v_key) {
    for (auto it = s_mounts.begin(); it != s_mounts.end(); ++it) {
        if (it->virt == v_key) { s_mounts.erase(it); }
    }
}

Option<Path> get_physical_path(const String& virtual_path) {
    for (auto it = s_mounts.begin(); it != s_mounts.end(); ++it) {
        if (it->virt == virtual_path) { return Some(it->pyhs); }
    }
    return None;
}

Option<Path> to_virtual(const Path& path, const std::string_view v_key) {
    if (is_virtual(path)) { return Some(path); }

    return get_physical_path(v_key).map(
        [&v_key, &path] (const Path& physical_path) {
            std::error_code ec;
            return v_key / std::filesystem::relative(path, physical_path, ec);
        }
    );
}

Option<Path> to_physical(const Path& path) {
    if (is_physical(path)) { return Some(path); }

    String p_str = path.string();

    return [] (const String& p) -> Option<Mount> {
        for (const auto& m : s_mounts) {
            if (p.starts_with(m.virt)) {
                return Some(m);
            }
        }
        return None;
    }(p_str).map(
        [&p_str] (const Mount& m) {
            const u32 idx = m.virt.length();
            return m.pyhs / p_str.erase(0, idx);
        }
    );
}

bool is_virtual(const Path& path) {
    for (const String p_str = path.string(); const auto& m : s_mounts) {
        if (p_str.starts_with(m.virt)) { return true; }
    }
    return false;
}

bool is_physical(const Path& path) { return path.is_absolute(); }

bool exists(const Path& path) {
    return to_physical(path).map(
        [] (const Path& p) {
            std::error_code ec;
            return std::filesystem::exists(p, ec);
        }
    ).value_or(false);
}

bool is_file(const Path& path) {
    return to_physical(path).map(
        [] (const Path& p) {
            std::error_code ec;
            return std::filesystem::is_regular_file(p, ec);
        }
    ).value_or(false);
}

bool is_dir(const Path& path) {
    return to_physical(path).map(
        [] (const Path& p) {
            std::error_code ec;
            return std::filesystem::is_directory(p, ec);
        }
    ).value_or(false);
}

Option<u64> get_file_size(const Path& path) {
    return to_physical(path).map(
        [] (const Path& p) {
            std::error_code ec;
            return std::filesystem::file_size(p, ec);
        }
    );
}

bool read_into(const Path& path, std::span<u8> buffer) {
    return to_physical(path).map(
        [&buffer] (const auto& p) {
            if (!is_file(p)) { return false; }
            File file{ p, FileOpenMode::Read };
            if (!file.can_read()) { return false; }
            file.read(buffer);
            return true;
        }
    ).value_or(false);
}

Option<Vector<u8>> read_bytes(const Path& path) {
    return get_file_size(path).map(
        [&path] (const u32 size) {
            Vector<u8> bytes(size);
            read_into(path, bytes);
            return bytes;
        }
    );
}

Option<String> read_text(const Path& path) {
    return get_file_size(path).map(
        [&path] (const u32 size) {
            String str;
            str.resize(size);
            read_into(path, std::span{ reinterpret_cast<u8*>(str.data()), size });
            return str;
        }
    );
}

bool write(const Path& path, const std::span<u8> buf) {
    auto file = open(path, FileOpenMode::Write);
    if (!file) { return false; }
    return file->write(buf);
}

bool write(const Path& path, const String& str) {
    auto file = open(path, FileOpenMode::Write);
    if (!file) { return false; }
    return file->write(std::span{ reinterpret_cast<const u8*>(str.data()), str.size() });
}

Option<File> open(const Path& path, FileOpenMode mode) {
    return to_physical(path).map(
        [mode] (const auto& p) {
            return std::move(File{ p, mode });
        }
    );
}
} // namespace siren::core::FileSystem
