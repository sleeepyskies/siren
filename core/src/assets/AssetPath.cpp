#include "AssetPath.hpp"


namespace siren::core
{
AssetPath::AssetPath() : m_buffer(nullptr), m_label_offset(0), m_path_offset(0) { }

AssetPath::AssetPath(
    const std::string_view vfs,
    const std::string_view relative_path,
    const std::string_view label
) : m_buffer(nullptr), m_label_offset(0), m_path_offset(0) {
    String full;
    full.reserve(vfs.size() + relative_path.size() + label.size() + 4);
    full += vfs;
    full += "://";
    full += relative_path;
    m_path_offset = vfs.size() + 3;
    if (!label.empty()) {
        full += "#";
        full += label;
        m_label_offset = m_path_offset + relative_path.size() + 1;
    }
    m_buffer = create_ref<const String>(std::move(full));
}

auto AssetPath::parse(const String& str) noexcept -> AssetPath {
    const auto colon = str.find("://");
    if (colon == String::npos) { return invalid(); }

    const auto pound = str.find('#');
    String vfs       = str.substr(0, colon);
    String path      = str.substr(
        colon + 3,
        pound == String::npos ? String::npos : pound - (colon + 3)
    );
    String label = pound == String::npos ? "no_label" : str.substr(pound + 1);
    return AssetPath{ std::move(vfs), std::move(path), std::move(label) };
}

auto AssetPath::invalid() noexcept -> AssetPath { return AssetPath{ }; }

auto AssetPath::is_valid() const noexcept -> bool { return m_buffer != nullptr; }

auto AssetPath::vfs() const -> String { return m_buffer->substr(0, m_path_offset); }

auto AssetPath::path() const -> String {
    return m_buffer->substr(m_path_offset, m_label_offset == 0 ? String::npos : m_label_offset - m_path_offset);
}

auto AssetPath::label() const -> Option<String> {
    if (m_label_offset == 0) { return Option<String>(None); }
    return Some(m_buffer->substr(m_label_offset + 1));
}

auto AssetPath::filename() const -> String { return Path{ path() }.filename().string(); }

auto AssetPath::extension() const -> String { return Path{ path() }.extension().string(); }

auto AssetPath::as_string() const noexcept -> std::string_view { return *m_buffer.get(); }

auto AssetPath::has_label() const noexcept -> bool { return m_label_offset != 0; }

auto AssetPath::hash() const noexcept -> HashedString { return HashedString{ as_string().data() }; }

} // namespace siren::core
