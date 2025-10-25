/**
 * @file ShaderImporter.hpp
 */
#pragma once

#include "utilities/spch.hpp"
#include "renderer/shaders/Shader.hpp"

namespace siren::core
{

/**
 * @brief Used to import Shader Groups, i.e. files with the .sshg extension.
 * A new ShaderImporter instance should be instantiated for each import.
 */
class ShaderImporter
{
public:
    /// @brief Creates a new ShaderImporter instance.
    static ShaderImporter create(const Path& path);

    /// @brief Loads and returns the shader. Returns nullptr on fail.
    Ref<Shader> load() const;

private:
    explicit ShaderImporter(const Path& path);
    Path m_path;
};

} // namespace siren::assets::importer
