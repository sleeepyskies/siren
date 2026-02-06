#pragma once
#include "Shader.hpp"

#include "core/FileSystem.hpp"


namespace siren::core
{
/**
 * @brief The ShaderLibrary is a cache used for core shaders only.
 */
class ShaderLibrary {
public:
    /// @brief Imports and caches the core shader.
    void import_shader(const Path& path, const std::string& alias);
    /// @brief Returns the core shader.
    std::shared_ptr<Shader> get(const std::string& name);
    /// @brief Reloads all shaders.
    void reload_shaders();
    /// @brief Reloads shader by name.
    void reload_shader(const std::string& name);

private:
    struct ShaderEntry {
        std::shared_ptr<Shader> shader;
        Path path;
    };

    std::unordered_map<std::string, ShaderEntry> m_cache; //< Core Shader handles cached.

    void reload(ShaderEntry& entry) const;
};
} // namespace siren::core
