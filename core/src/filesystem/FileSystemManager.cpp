#include "../core/FileSystemManager.hpp"

// HACK: this is platform specific, so we should introduce platform detection, but for now is ok
#include <fstream>
#include <windows.h>

namespace siren::core
{

namespace fs = std::filesystem;

static HashMap<AccessType, std::string> s_accessToString{
    { AccessType::ASSETS, "ass://" },
    { AccessType::ENGINE, "eng://" },
    { AccessType::FILESYSTEM, "" },
};

FileSystemManager::FileSystemManager()
{
    // determine assets root
    const Path cwd = std::filesystem::current_path();
    Path directory = cwd;
    while (exists(directory) && isDirectory(directory)) {
        const Path checkPath = directory / "assets";

        // success! we have found the asset directory
        if (fs::exists(checkPath) && fs::is_directory(checkPath)) {
            m_assetsRoot = checkPath;
            break;
        }

        // we need to keep looking...
        if (directory.has_parent_path()) {
            directory = directory.parent_path();
            continue;
        }

        // cannot search anymore and have not found, use fallback
        wrn("Could not successfully find asset root directory, using fallback {}", cwd.string());
        m_assetsRoot = cwd;
    }

    // atm, engine root is parent of assets root
    m_engineRoot = m_assetsRoot.parent_path();
}

const Path& FileSystemManager::getEngineRoot() const
{
    return m_engineRoot;
}

const Path& FileSystemManager::getAssetsRoot() const
{
    return m_assetsRoot;
}

Path FileSystemManager::resolveVirtualPath(const Path& virtualPath) const
{
    const std::string pathString = virtualPath.string();

    if (pathString.starts_with(s_accessToString[AccessType::ASSETS])) {
        return m_assetsRoot / pathString.substr(s_accessToString[AccessType::ASSETS].size());
    }
    if (pathString.starts_with(s_accessToString[AccessType::ENGINE])) {
        return m_engineRoot / pathString.substr(s_accessToString[AccessType::ENGINE].size());
    }

    return virtualPath;
}

Path FileSystemManager::makeAbsolute(const Path& path) const
{
    if (path.is_absolute()) { return path; }
    const Path path_ = resolveVirtualPath(path);
    if (path_.is_absolute()) { return path; }
    return m_engineRoot / path_;
}

Path FileSystemManager::makeRelative(const Path& path, const AccessType accessType) const
{
    const std::string pathStr = path.string();

    // already relative
    if (pathStr.starts_with(s_accessToString[accessType])) { return path; }

    Path path_;
    switch (accessType) {
        case AccessType::ASSETS    : path_ = fs::relative(path, m_assetsRoot); break;
        case AccessType::ENGINE    :
        case AccessType::FILESYSTEM: path_ = fs::relative(path, m_engineRoot); break;
    }
    return path_.lexically_normal();
}

bool FileSystemManager::exists(const Path& path) const
{
    const Path path_ = resolveVirtualPath(path);
    return fs::exists(path_);
}

bool FileSystemManager::isFile(const Path& path) const
{
    const Path path_ = resolveVirtualPath(path);
    // we accept regular and binary files
    return fs::is_regular_file(path_) || fs::is_character_file(path_);
}

bool FileSystemManager::isDirectory(const Path& path) const
{
    const Path path_ = resolveVirtualPath(path);
    return fs::is_directory(path_);
}

std::string FileSystemManager::readFile(const Path& path) const
{
    const Path path_ = resolveVirtualPath(path);
    if (!isFile(path_)) { return ""; }

    // read whole file in one go using file size, avoids dynamic string resizing, but may lead to
    // huge allocated blocks of memory if file is very large
    const auto size = fs::file_size(path_);
    std::string data(size, '\0');
    std::ifstream in(path_);
    in.read(&data[0], size);

    return data;
}

void FileSystemManager::writeFile(const Path& path, const std::string& data) const
{
    const Path path_ = resolveVirtualPath(path);
    if (isFile(path_)) { return; }

    // create any missing directories
    if (!isDirectory(path_.parent_path())) { fs::create_directories(path_.parent_path()); }

    std::ofstream ofs(path_, std::ios::binary);
    ofs.write(data.data(), data.size());
}

void FileSystemManager::overwriteFile(const Path& path, const std::string& data) const
{
    const Path path_ = resolveVirtualPath(path);

    // create any missing directories
    if (!isDirectory(path_.parent_path())) { fs::create_directories(path_.parent_path()); }

    std::ofstream ofs(path_, std::ios::binary);
    ofs.write(data.data(), data.size());
}

} // namespace siren::core