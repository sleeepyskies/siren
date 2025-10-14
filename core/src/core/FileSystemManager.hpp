#pragma once

#include "utilities/spch.hpp"

namespace siren::core
{

// HACK: Siren reads all data from disk, this means no data is embedded into the exe. For user
// files, this is fine, but for core siren resources, we should really embed this data into the exe.
// However, this is a future improvement that can be done.

/**
 * @brief Enum indicating what type of file/dir access we want to perform. May either by a form of
 * virtual path access, or standard file system acess
 */
enum class AccessType {
    ASSETS, // "ass://"
    ENGINE, // "eng://"
    FILESYSTEM
};

/**
 * @brief A utility class for handling file paths. Siren makes use of virtual file paths,
 * meaning that certain prefixes to paths map to predefined directories.
 */
class FileSystemManager
{
public:
    /// @brief Initializes the PathRegistry and finds the project and engine root
    FileSystemManager();
    ~FileSystemManager() = default;

    const Path& getEngineRoot() const;
    const Path& getAssetsRoot() const;
    Path resolveVirtualPath(const Path& virtualPath) const;
    Path makeAbsolute(const Path& path) const;
    Path makeRelative(const Path& path, AccessType accessType) const;
    bool exists(const Path& path) const;
    bool isFile(const Path& path) const;
    bool isDirectory(const Path& path) const;
    std::string readFile(const Path& path) const;
    void writeFile(const Path& path, const std::string& data) const;
    void overwriteFile(const Path& path, const std::string& data) const;

private:
    Path m_engineRoot{}; // "eng://" do we need atm?
    Path m_assetsRoot{}; // "ass://"
};

} // namespace siren::core
