#include "AssetLoader.hpp"
#include "stb_image.h"

#include "assets/AssetServer.hpp"
#include "core/FileSystem.hpp"
#include "glm/gtc/integer.hpp"


namespace siren::core
{

auto TextureLoader::load(LoadContext&& ctx, const LoaderConfig& config) const -> std::expected<void, Error> {
    if (!std::holds_alternative<TextureLoaderConfig>(config)) { return std::unexpected(Error(Code::LogicFail)); }
    // const cast bc we need to move sampler out of the config....
    auto& config_ = const_cast<TextureLoaderConfig&>(std::get<TextureLoaderConfig>(config));

    struct Raw {
        std::vector<u8> buffer;
        i32 w;
        i32 h;
        i32 channels;
    };

    // parse raw img data
    const auto data_opt = FileSystem::to_physical(ctx.path().path()).and_then(
        [] (const Path& p) -> std::optional<Raw> {
            i32 w, h, c;
            stbi_set_flip_vertically_on_load(true);
            u8* img_data = stbi_load(p.string().c_str(), &w, &h, &c, STBI_default);
            if (!img_data) { return std::nullopt; }
            const auto raw = Raw{
                .buffer = std::vector<u8>{ img_data, img_data + w * h * c },
                .w = w,
                .h = h,
                .channels = c
            };
            stbi_image_free(img_data);
            return raw;
        }
    );

    if (!data_opt.has_value()) { return std::unexpected(Error{ Code::IOFail }); }
    Raw raw = data_opt.value();

    ImageFormat image_format = config_.format.or_else(
        [&config_, &raw]() -> std::optional<ImageFormat> {
            if (raw.channels != 4) return ImageFormat::Color8;
            return config_.is_srgb ? ImageFormat::Color8 : ImageFormat::LinearColor8;
        }
    ).value();

    ImageExtent image_extent = config_.array_layout.transform(
        [&raw] (const ImageArrayLayout& layout) -> ImageExtent {
            return ImageExtent{
                .width = (u32)raw.w,
                .height = layout.row_height,
                .depth_or_layers = (u32)raw.h / layout.row_height,
            };
        }
    ).value_or(
        ImageExtent{
            .width = (u32)raw.w,
            .height = (u32)raw.h,
            .depth_or_layers = 1,
        }
    );

    // todo: D1, D3 and Cube support
    //ImageDimension image_dimension = []() -> ImageDimension {
    //if (conf.is_cube &&) { return ImageDimension::Cube; }
    //}();

    const u32 max_dim       = glm::max(image_extent.width, glm::max(image_extent.height, image_extent.depth_or_layers));
    const u32 mipmap_levels = config_.generate_mipmap_levels
                                  ? 1 + static_cast<u32>(glm::floor(glm::log2(max_dim)))
                                  : 0;

    ctx.finish<Texture>(
        std::make_unique<Texture>(
            config_.name.value_or(ctx.path().filename()),
            Image{
                raw.buffer,
                image_format,
                image_extent,
                ImageDimension::D2,
                mipmap_levels,
            },
            std::move(config_.sampler)
        )
    );

    return { };
}

} // namespace siren::core
