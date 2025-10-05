#include "ShaderImporter.hpp"

#include "json.hpp"
#include <fstream>

namespace siren::assets::ShaderImporter
{

std::string loadFile(const fs::path& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        err("Could not open file {}", path.string());
        return {};
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    nfo("Loaded file {}", path.string());
    return ss.str();
}

Ref<renderer::Shader> importShader(const fs::path& path)
{
    if (!exists(path)) {
        wrn("File does not exist at {}", path.string());
        return nullptr;
    }

    if (path.extension().string() != ".sshg") {
        wrn("Invalid shader group file format at {}", path.string());
        return nullptr;
    }

    std::ifstream file(path.string());
    if (!file || !file.is_open()) {
        wrn("Failed to open file at {}", path.string());
        return nullptr;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    const std::string jsonString = buffer.str();
    auto data = nlohmann::json::parse(jsonString.begin(), jsonString.end(), nullptr, true);

    // currently, sshg only has name, and a vertex and frag shader, which are all required
    if (!data.contains("name") || !data.contains("stages")) {
        wrn("Invalid sshg syntax in file at {}", path.string());
        return nullptr;
    }

    const auto& stages = data["stages"];

    if (!stages.contains("vertex") || !stages.contains("fragment")) {
        wrn("Invalid sshg syntax in file at {}", path.string());
        return nullptr;
    }

    fs::path vertexPath   = (path.parent_path()) / stages["vertex"].get<std::string>();
    fs::path fragmentPath = (path.parent_path()) / stages["fragment"].get<std::string>();

    if (!exists(vertexPath) || !exists(fragmentPath)) {
        wrn("sshg file names invalid shader files at {}", path.string());
        return nullptr;
    }

    std::string vertexString   = loadFile(vertexPath);
    std::string fragmentString = loadFile(fragmentPath);
    std::string name           = data["name"];

    return makeRef<renderer::Shader>(name, vertexString, fragmentString);
}
} // namespace siren::assets::ShaderImporter
