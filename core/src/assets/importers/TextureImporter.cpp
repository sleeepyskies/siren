#include "TextureImporter.hpp"

#include "../../../../cmake-build-debug/_deps/assimp-src/code/Common/ScenePrivate.h"
#include "filesystem/FileSystemModule.hpp"
#include "renderer/Texture.hpp"
#include <assimp/texture.h>

#include <stb_image.h>

namespace siren::core
{

// ============================================================================
// == MARK: Builder Functions
// ============================================================================

TextureImporter TextureImporter::create(const Path& path)
{
    return TextureImporter(path);
}

TextureImporter TextureImporter::create(const aiScene* aiScene, const aiString& aiString)
{
    return TextureImporter({aiScene, aiString});
}

TextureImporter& TextureImporter::setSampler(const Texture2DSampler& sampler)
{
    m_sampler = sampler;
    return *this;
}

TextureImporter::TextureImporter(const Path& path) : m_source(path)
{
}

TextureImporter::TextureImporter(const AssimpSource& source) : m_source(source)
{
}


// ============================================================================
// == MARK: Import Logic
// ============================================================================


Ref<Texture2D> TextureImporter::load()
{
    const auto visitor = [this]<typename TArg>(TArg&&) -> Ref<Texture2D> {
        using T = std::decay_t<TArg>;

        if constexpr (std::is_same_v<T, Path>) {
            return loadFromPath();
        } else if constexpr (std::is_same_v<T, AssimpSource>) {
            return loadFromAssimp();
        }
        SirenAssert(false, "Invalid TextureImporter Source type encountered");
    };

    return std::visit(visitor, m_source);

}

Ref<Texture2D> TextureImporter::loadFromPath() const
{
    const Path path = std::get<Path>(m_source);

    if (!filesystem().exists(path)) {
        wrn("File does not exist at {}", path.string());
        return nullptr;
    }

    stbi_set_flip_vertically_on_load(true);
    i32 w, h, c;
    const stbi_uc* data = stbi_load(absolute(path).string().data(), &w, &h, &c, STBI_default);

    if (!data) {
        wrn("Could not load image at {}", path.string());
        return nullptr;
    }

    const std::vector<u8> buf{data, data + w * h * c};

    const std::string name = path.filename().string();

    return createRef<Texture2D>(name, buf, m_sampler, w, h);
}

Ref<Texture2D> TextureImporter::loadFromAssimp()
{
    // Textures can either be embedded or external.
    //  - External textures are simply defined by a filepath.
    //  - Embedded textures are identified via strings like "*0", "*1", and can either be
    //      - uncompressed
    //      - compressed (in this case, mHeight is 0)
    const auto [aiScene, aiString] = std::get<AssimpSource>(m_source);

    const std::string texturePath = aiString.C_Str();

    if (!texturePath.starts_with('*')) {
        // we have an external texture
        m_source = Path(texturePath);
        return loadFromPath();
    }

    // this is an embedded texture
    const u32 index            = std::stoi(texturePath.substr(1));
    const aiTexture* aiTexture = aiScene->mTextures[index];

    i32 width  = aiTexture->mWidth;
    i32 height = aiTexture->mHeight;

    std::vector<u8> imgData{};

    if (height == 0) {
        // compressed data
        const auto compressedData = reinterpret_cast<const stbi_uc*>(aiTexture->pcData);
        i32 w, h, c;
        stbi_uc* raw = stbi_load_from_memory(compressedData, width, &w, &h, &c, STBI_default);
        imgData      = std::vector(raw, raw + (w * h * c));
        width        = w;
        height       = h;
        stbi_image_free(raw);
    } else {
        // uncompressed data
        for (i32 i = 0; i < width * height; i++) {
            const auto& [b, g, r, a] = aiTexture->pcData[i];
            imgData.push_back(r);
            imgData.push_back(g);
            imgData.push_back(b);
            imgData.push_back(a);
        }
    }

    const std::string name = aiTexture->mFilename.C_Str();

    return createRef<Texture2D>(name, imgData, m_sampler, width, height);
}

} // namespace siren::assets::importer