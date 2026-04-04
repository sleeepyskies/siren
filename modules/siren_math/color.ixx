export module siren.color;

import siren.math;

namespace siren {

/// @todo: flesh this out more

/**
 * @struct RGBA
 * @brief A four channel color.
 */
struct RGBA : glm::vec4 {
    using glm::vec4::vec4;

    /** @brief A pure black color. */
    static inline const RGBA BLACK = { 0.f, 0.f, 0.f, 1.f };
    /** @brief A pure white color. */
    static inline const RGBA WHITE = { 1.f, 1.f, 1.f, 1.f };
    /** @brief A pure red color. */
    static inline const RGBA RED = { 1.f, 0.f, 0.f, 1.f };
    /** @brief A pure green color. */
    static inline const RGBA GREEN = { 0.f, 1.f, 0.f, 1.f };
    /** @brief A pure blue color. */
    static inline const RGBA BLUE = { 0.f, 0.f, 1.f, 1.f };

    static constexpr auto lerp(const RGBA& other) -> RGBA;
};

} // namespace siren
