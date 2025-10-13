#pragma once

#include "renderer/shaders/Shader.hpp"
#include "utilities/spch.hpp"

namespace siren::assets::ShaderImporter
{

Ref<renderer::Shader> importShader(const Path& path);

} // namespace siren::assets::ShaderImporter
