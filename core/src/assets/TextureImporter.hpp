#pragma once

#include "renderer/Texture.hpp"
#include "utilities/spch.hpp"

namespace core::assets::TextureImporter
{

Ref<renderer::Texture2D> importTexture2D(const fs::path& path);

} // namespace core::assets::TextureImporter
