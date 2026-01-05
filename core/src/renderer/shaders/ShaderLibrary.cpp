#include "ShaderLibrary.hpp"

#include <ranges>

#include "assets/importers/ShaderImporter.hpp"
#include "filesystem/FileSystemModule.hpp"


namespace siren::core
{
void ShaderLibrary::Import(const Path& path, const std::string& alias)
{
    const auto resolvedPath = filesystem().ResolveVirtualPath(path);
    const auto importer     = ShaderImporter::Create(filesystem().ResolveVirtualPath(path));
    const auto shader       = importer.Load();
    m_cache[alias]          = ShaderEntry{ .shader = shader, .path = resolvedPath };
}

Ref<Shader> ShaderLibrary::Get(const std::string& name)
{
    const auto it = m_cache.find(name);
    if (it == m_cache.end()) return nullptr;
    return it->second.shader;
}

void ShaderLibrary::ReloadShaders()
{
    for (auto& shader : m_cache | std::ranges::views::values) { Reload(shader); }
}

void ShaderLibrary::ReloadShader(const std::string& name)
{
    const auto it = m_cache.find(name);
    if (it == m_cache.end()) return;
    return Reload(it->second);
}

void ShaderLibrary::Reload(ShaderEntry& entry) const
{
    // try to build the shader again
    if (!entry.shader) {
        const auto shader = ShaderImporter::Create(entry.path).Load();
        if (shader) { entry.shader = shader; }
        return;
    }

    const auto source = ShaderImporter::Create(entry.path).LoadSourceStrings();
    if (!source) {
        wrn("Shader recompilation of {} failed", entry.path.string());
        return;
    }

    entry.shader->Recompile(source->vertex, source->fragment);
}
} // namespace siren::core
