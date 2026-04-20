module;

#include <utility>

export module siren.ecs:system;

import siren.common;

namespace siren::ecs {

export {

    /**
     * @class Resource
     * @brief A non owning wrapper providing pointer like access to some resource T.
     *
     * Can be accessed from systems iff this resource type has been
     * injected into the @ref World.
     *
     * @tparam T The type of resource wrapped.
     */
    template <typename T>
    class Resource {
    public:
        /** @brief The raw stripped type. */
        using ValueType = std::remove_pointer_t<std::remove_cvref_t<T>>;
        /** @brief Pointer to the resource. */
        using PointerType = ValueType*;
        /** @brief Reference to the resource. */
        using ReferenceType = ValueType&;

        /**
         * @brief Takes a reference to a resource and provides access to it.
         * @param resource The resource to wrap.
         */
        explicit Resource(ReferenceType resource) : m_resource(resource) { }

        /**
         * @brief Access the resource pointer.
         * @return Pointer to the resource.
         */
        template <typename Self>
        [[nodiscard]] constexpr auto operator->(this Self&& self) noexcept -> PointerType {
            return &std::forward<Self>(self).m_resource;
        }

        /**
         * @brief Access the resource reference.
         * @return Reference to the resource.
         */
        template <typename Self>
        [[nodiscard]] constexpr auto operator*(this Self&& self) noexcept -> ReferenceType {
            return std::forward<Self>(self).m_resource;
        }

    private:
        /** @brief The inner resource. */
        ReferenceType m_resource;
    };
}


export {
    template <typename T>
    struct IsResource : std::false_type { };

    template <typename T>
    struct IsResource<Resource<T>> : std::true_type { };

    template <typename T>
    inline constexpr bool IsResource_v = IsResource<T>::value;

    template <typename T>
    struct ResourceTraits { };

    template <typename T>
    struct ResourceTraits<Resource<T>> {
        using Inner = T;
    };
}

} // namespace siren::ecs

