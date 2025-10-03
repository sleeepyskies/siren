#pragma once

#include "renderer/Shader.hpp"
#include "utilities/spch.hpp"

namespace core::assets::ShaderImporter
{

Ref<renderer::Shader> importShader(const fs::path& path);

} // namespace core::assets::ShaderImporter
