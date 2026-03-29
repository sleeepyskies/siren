module;

#include <string_view>
#include <optional>
#include <memory>

export module siren.asset.asset_path;

import siren.common;
import siren.io.file;

namespace siren::asset {

/**
 * @brief A lightweight shared representation of an assets location in a VFS.
 *
 * AssetPath's follow the structure:
 *
 * @c "vfs://relative/asset/path/file.sr#some_label"
 *
 * Labels are not required. The presence of a label indicates the asset
 * this path refers to is embedded into another assets file.
 */
export class AssetPath {
public:
    /// @brief Constructs an invalid AssetPath.
    AssetPath();

    /**
     * @brief
     * @param vfs The virtual file system mount.
     * @param relative_path The path relative to the virtual file system mount.
     * @param label The optional label of the asset.
     */
    AssetPath(
        std::string_view vfs,
        std::string_view relative_path,
        std::string_view label = ""
    );

    /**
     * @brief Parses an AssetPath from its string representation.
     *
     * Examples of accepted input string formats
     *     - @c "vfs://relative/asset/path/file.sr#some_label"
     *     - @c "vfs://relative/asset/path/file.sr"
     *
     * Examples of invalid formats
     *     - @c "relative/asset/path/file.sr#some_label"
     *     - @c "vfs://relative/asset/path/file.sr#label#more_labels#"
     *     - @c "://relative/asset/path/file.sr#label#more_labels#"
     *
     * @param str Input string in AssetPath format
     * @return Parsed AssetPath or invalid() on failure
     */
    [[nodiscard]]
    auto static parse(const std::string& str) noexcept -> AssetPath;
    /// @brief Returns an invalid AssetPath.
    [[nodiscard]]
    auto static invalid() noexcept -> AssetPath;

    /// @brief Checks if the AssetPath is valid or not.
    [[nodiscard]]
    auto is_valid() const noexcept -> bool;

    /// @brief Returns the VFS mount.
    [[nodiscard]]
    auto vfs() const -> std::string;
    /// @brief Returns the relative path.
    [[nodiscard]]
    auto path() const -> std::string;
    /// @brief Returns the label, if present.
    [[nodiscard]]
    auto label() const -> std::optional<std::string>;

    /// @brief Returns the filename of this AssetPath.
    [[nodiscard]]
    auto filename() const -> std::string;
    [[nodiscard]]
    /// @brief Returns the file extension of this AssetPath.
    auto extension() const -> std::string;
    /// @brief Returns the full string representation of this AssetPath.
    [[nodiscard]]
    auto as_string() const noexcept -> std::string_view;

    /// @brief Checks if the AssetPath has a label.
    [[nodiscard]]
    auto has_label() const noexcept -> bool;
    /// @brief Returns the @ref HashedString of this AssetPath.
    [[nodiscard]]
    auto hashed_string() const noexcept -> HashedString;

private:
    /// @brief The underlying string buffer.
    /// @details
    ///  We use a single shared string buffer here to
    ///  reduce memory overhead, since WeakHandle and StrongHandle
    ///  have an AssetPath and are copied and passed around alot.
    std::shared_ptr<const std::string> m_buffer;
    /// @brief The offset of the label into the main buffer.
    /// Is not required, in which case it is set to 0.
    /// @code
    /// "ass://path/to/file.sr#label"
    ///                        ^
    /// m_label_offset == 22
    /// @endcode
    u16 m_label_offset;
    /// @brief The offset of the path into the main buffer.
    /// @code
    /// "ass://path/to/file.sr#label"
    ///        ^
    /// m_path_offset == 6
    /// @endcode
    u16 m_path_offset;
};

AssetPath::AssetPath() : m_buffer(nullptr), m_label_offset(0), m_path_offset(0) { }

AssetPath::AssetPath(
    const std::string_view vfs,
    const std::string_view relative_path,
    const std::string_view label
) : m_buffer(nullptr), m_label_offset(0), m_path_offset(0) {
    std::string full;
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
    m_buffer = std::make_shared<const std::string>(std::move(full));
}

auto AssetPath::parse(const std::string& str) noexcept -> AssetPath {
    const auto colon = str.find("://");
    if (colon == std::string::npos) { return invalid(); }

    const auto pound = str.find('#');
    std::string vfs  = str.substr(0, colon);
    std::string path = str.substr(
        colon + 3,
        pound == std::string::npos ? std::string::npos : pound - (colon + 3)
    );
    std::string label = pound == std::string::npos ? "no_label" : str.substr(pound + 1);
    return AssetPath{ std::move(vfs), std::move(path), std::move(label) };
}

auto AssetPath::invalid() noexcept -> AssetPath { return AssetPath{ }; }

auto AssetPath::is_valid() const noexcept -> bool { return m_buffer != nullptr; }

auto AssetPath::vfs() const -> std::string { return m_buffer->substr(0, m_path_offset); }

auto AssetPath::path() const -> std::string {
    return m_buffer->substr(m_path_offset, m_label_offset == 0 ? std::string::npos : m_label_offset - m_path_offset);
}

auto AssetPath::label() const -> std::optional<std::string> {
    if (m_label_offset == 0) { return std::nullopt; }
    return m_buffer->substr(m_label_offset + 1);
}

auto AssetPath::filename() const -> std::string { return io::Path{ path() }.filename().string(); }

auto AssetPath::extension() const -> std::string { return io::Path{ path() }.extension().string(); }

auto AssetPath::as_string() const noexcept -> std::string_view { return *m_buffer.get(); }

auto AssetPath::has_label() const noexcept -> bool { return m_label_offset != 0; }

auto AssetPath::hashed_string() const noexcept -> HashedString {
    if (!m_buffer) { return HashedString{ }; }
    return HashedString{ m_buffer.get()->data() };
}

} // namespace siren::asset
