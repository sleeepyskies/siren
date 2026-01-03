#include "ShaderImporter.hpp"

#include "filesystem/FileSystemModule.hpp"
#include "node.hpp" // from fkYAML


namespace siren::core
{

ShaderImporter ShaderImporter::create(const Path& path)
{
    return ShaderImporter(path);
}

Ref<Shader> ShaderImporter::load() const
{
    const auto fs = filesystem();

    if (!fs.exists(m_path)) {
        wrn("File does not exist at {}", m_path.string());
        return nullptr;
    }

    const std::string yamlString = fs.readFile(m_path);
    auto node                    = fkyaml::node::deserialize(yamlString);

    // currently, sshg only has name, and a vertex and frag shader, which are all required
    const std::string name           = node["name"].get_value<std::string>();
    const std::string vertexSource   = node["stages"]["vertex"].get_value<std::string>();
    const std::string fragmentSource = node["stages"]["fragment"].get_value<std::string>();

    Path vertexPath   = m_path.parent_path() / vertexSource;
    Path fragmentPath = m_path.parent_path() / fragmentSource;

    if (!fs.exists(vertexPath) || !fs.exists(fragmentPath)) {
        wrn("sshg file names invalid shader files at {}", m_path.string());
        return nullptr;
    }

    std::string vertexString   = fs.readFile(vertexPath);
    std::string fragmentString = fs.readFile(fragmentPath);

    dbg("Loaded Shader {}", m_path.string());
    return CreateRef<Shader>(name, vertexString, fragmentString);
}

ShaderImporter::ShaderImporter(const Path& path) : m_path(path) { }

} // namespace siren::assets::importer
