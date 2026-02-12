#include "shader_library.hpp"

#include <ranges>

#include "assets/asset_server.hpp"

#include "core/file_system.hpp"


namespace siren::core
{
void ShaderLibrary::import_shader(const Path& path, const std::string& alias) {
    Locator<AssetServer>::locate().load<Shader>(AssetPath::parse(path.string()));
    m_cache[alias] = ShaderEntry{ .shader = shader, .path = resolved_path };
}

std::shared_ptr<Shader> ShaderLibrary::get(const std::string& name) {
    const auto it = m_cache.find(name);
    if (it == m_cache.end()) return nullptr;
    return it->second.shader;
}

void ShaderLibrary::reload_shaders() {
    for (auto& shader : m_cache | std::ranges::views::values) { reload(shader); }
}

void ShaderLibrary::reload_shader(const std::string& name) {
    const auto it = m_cache.find(name);
    if (it == m_cache.end()) return;
    return reload(it->second);
}

void ShaderLibrary::reload(ShaderEntry& entry) const {
    // try to build the shader again
    if (!entry.shader) {
        const auto shader = ShaderImporter::Create(entry.path).Load();
        if (shader) { entry.shader = shader; }
        return;
    }

    const auto source = ShaderImporter::Create(entry.path).LoadSourceStrings();
    if (!source) {
        Logger::renderer->warn("Shader recompilation of {} failed", entry.path.string());
        return;
    }

    entry.shader->compile(source->vertex, source->fragment);
}
} // namespace siren::core
