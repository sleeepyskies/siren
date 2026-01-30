#include "AssetLoader.hpp"
#include "stb_image.h"

#include "assets/AssetServer.hpp"
#include "core/FileSystem.hpp"
#include "glm/gtc/integer.hpp"


namespace siren::core
{

auto TextureLoader::load(LoadContext&& ctx, const LoaderConfig& config) const -> Result<Unit, Error> {
    // const cast bc we need to move the img sampler into the final img, it's a gpu resource so we cant copy
    auto& conf = *const_cast<TextureLoaderConfig*>(dynamic_cast<const TextureLoaderConfig*>(&config));

    struct Raw {
        Vector<u8> buffer;
        i32 w;
        i32 h;
        i32 channels;
    };

    // parse raw img data
    const auto data_opt = FileSystem::to_physical(ctx.path().path()).and_then(
        [] (const Path& p) -> Option<Raw> {
            i32 w, h, c;
            stbi_set_flip_vertically_on_load(true);
            u8* img_data = stbi_load(p.string().c_str(), &w, &h, &c, STBI_default);
            if (!img_data) { return None; }
            const auto raw = Some(
                Raw{ .buffer = Vector<u8>{ img_data, img_data + w * h * c }, .w = w, .h = h, .channels = c }
            );
            stbi_image_free(img_data);
            return raw;
        }
    );

    if (data_opt.is_none()) { return Err{ Error{ Code::AssetIOFail } }; }
    Raw raw = data_opt.unwrap();

    ImageFormat image_format = conf.format.or_else(
        [&conf, &raw]() {
            if (raw.channels != 4) { return ImageFormat::Color8; }
            if (conf.is_srgb) {
                return ImageFormat::Color8;
            }
            return ImageFormat::LinearColor8;
        }
    );

    ImageExtent image_extent = conf.array_layout.map(
        [&raw] (const ImageArrayLayout& layout) -> ImageExtent {
            return ImageExtent{
                .width = (u32)raw.w,
                .height = layout.row_height,
                .depth_or_layers = raw.h / layout.row_height,
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

    const u32 mipmap_levels =
            conf.generate_mipmap_levels
                ? 1 + glm::floor(
                    glm::log2(
                        glm::max(image_extent.width, glm::max(image_extent.height, image_extent.depth_or_layers))
                    )
                )
                : 0;

    ctx.finish(
        Texture{
            conf.name.value_or(ctx.path().filename()),
            Image{
                raw.buffer,
                image_format,
                image_extent,
                ImageDimension::D2,
                mipmap_levels,
            },
            std::move(conf.sampler)
        }
    );

    return Ok(unit);
}

} // namespace siren::core
