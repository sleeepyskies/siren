#pragma once

#include "type_name.hpp"
#include "core/assert.hpp"


namespace siren::core
{
/**
 * @class Locator
 * @brief A service locator.
 *
 * Siren makes use of singletons, meaning single instances of some class
 * used throughout the engine. However, raw singletons come with some
 * issues such as initialization order, testing and lifetime. A service
 * locator helps with these a little.
 *
 * @tparam Service The service type.
 */
template <typename Service>
class Locator {
public:
    Locator()                          = delete;
    ~Locator()                         = delete;
    Locator(const Locator&)            = delete;
    Locator(Locator&&)                 = delete;
    Locator& operator=(const Locator&) = delete;
    Locator& operator=(Locator&&)      = delete;

    /**
     * @brief Returns a reference to the held service.
     * @return A reference to the inner service.
     */
    static auto value() -> Service& {
        SIREN_ASSERT(has_value(), "Cannot locate {}, it has not been provided.", type_name<Service>());
        return *static_cast<Service*>(m_service.get());
    }

    /**
     * @brief Checks whether this Locator contains a service.
     * @return Whether this Locator contains a service.
     */
    [[nodiscard]] static auto has_value() noexcept -> bool {
        return m_service != nullptr;
    }

    /**
     * @brief Constructs or overwrites a new Service within this locator.
     * @tparam Args Arguments required by the Service.
     * @param args The arguments passed to construct the Service.
     */
    template <typename... Args>
    static void emplace(Args&&... args) {
        m_service = std::make_unique<Service>(std::forward<Args>(args)...);
    }

    /** @brief Resets the service. */
    static void reset() {
        m_service.reset();
    }

    /** @brief The contained service. */
    static inline std::unique_ptr<Service> m_service{ };
};
} // namespace siren::core

