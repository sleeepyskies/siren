#pragma once
#include <glm/vec4.hpp>


namespace siren::core
{

// todo: flesh this out a lil. maybe conversions? maybe more color types?

struct RGB;
struct RGBA;

/**
 * @struct RGBA
 * @brief Represents an RGBA color. Internally just a glm::vec4.
 */
struct RGBA : glm::vec4 {
    using glm::vec4::vec4;

    static const RGBA BLACK;
    static const RGBA WHITE;
    static const RGBA RED;
    static const RGBA GREEN;
    static const RGBA BLUE;

    auto to_rgb() const noexcept -> RGB;
    auto from_vec4() const noexcept -> RGBA;
};

inline const RGBA RGBA::BLACK{ 0.f, 0.f, 0.f, 1.f };
inline const RGBA RGBA::WHITE{ 1.f, 1.f, 1.f, 1.f };
inline const RGBA RGBA::RED{ 1.f, 0.f, 0.f, 1.f };
inline const RGBA RGBA::GREEN{ 0.f, 1.f, 0.f, 1.f };
inline const RGBA RGBA::BLUE{ 0.f, 0.f, 1.f, 1.f };

/**
 * @struct RGB
 * @brief Represents an RGB color. Internally just a glm::vec3.
 */
struct RGB : glm::vec3 {
    using glm::vec3::vec3;
};

} // namespace siren::core
