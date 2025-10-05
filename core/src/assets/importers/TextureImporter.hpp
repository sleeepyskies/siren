#pragma once

#include "renderer/Texture.hpp"
#include "utilities/spch.hpp"

namespace siren::assets::TextureImporter
{

Ref<renderer::Texture2D> importTexture2D(const fs::path& path);

} // namespace siren::assets::TextureImporter
