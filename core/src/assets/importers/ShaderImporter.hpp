#pragma once

#include "renderer/Shader.hpp"
#include "utilities/spch.hpp"

namespace siren::assets::ShaderImporter
{

Ref<renderer::Shader> importShader(const fs::path& path);

} // namespace siren::assets::ShaderImporter
