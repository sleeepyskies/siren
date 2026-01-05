/**
 * @file ShaderImporter.hpp
 */
#pragma once

#include "utilities/spch.hpp"
#include "renderer/shaders/Shader.hpp"


namespace siren::core
{
struct ShaderSourceStrings
{
    std::string name;
    std::string vertex;
    std::string fragment;
};

/**
 * @brief Used to import Shader Groups, i.e. files with the .sshg extension.
 * A new ShaderImporter instance should be instantiated for each import.
 */
class ShaderImporter
{
public:
    /// @brief Creates a new ShaderImporter instance.
    static ShaderImporter Create(const Path& path);

    /// @brief Loads and returns the shader. Returns nullptr on fail.
    Ref<Shader> Load() const;
    /// @brief Loads and returns the shader source strings. Returns nullptr on fail.
    Maybe<ShaderSourceStrings> LoadSourceStrings() const;

private:
    explicit ShaderImporter(const Path& path);
    Path m_path;
};
} // namespace siren::assets::importer
