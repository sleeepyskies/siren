module;

#include <entt/entt.hpp>

export module siren.ecs.system;

import siren.common;

namespace siren::ecs {

export {
    template <typename T>
    concept IsSystem = requires() { };
}


export {
    /// @todo More complete API for Query, but for now we just simply wrap an entt view.

    template <IsReference... Args>
    class Query {
        using ArgsPack = std::tuple<Args...>;

    public:
        explicit Query(entt::registry& registry) : m_view(registry.view<Args...>()) { }

        auto view() -> entt::view<Args...> { return m_view; }

    private:
        entt::view<Args...> m_view;
    };

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
        requires IsReference<T>
    class Resource {
    public:
        /// @brief The raw stripped type.
        using UnderlyingType = std::remove_pointer_t<std::remove_cvref_t<T>>;
        /// @brief Pointer to the resource.
        using PointerType = UnderlyingType*;
        /// @brief Reference to the resource.
        using ReferenceType = UnderlyingType&;

        /**
         * @brief Takes a reference to a resource and provides access to it.
         * @param resource The resource to wrap.
         */
        explicit Resource(ReferenceType resource) : m_resource(std::move(resource)) { }

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
    struct IsQuery : std::false_type { };

    template <typename... Args>
    struct IsQuery<Query<Args...>> : std::true_type { };

    template <typename... Args>
    inline constexpr bool IsQuery_v = IsQuery<Args...>::value;

    template <typename T>
    struct QueryTraits { };

    template <typename... Args>
    struct QueryTraits<Query<Args...>> {
        using ArgsTuple                  = std::tuple<Args...>;
        static constexpr usize ArgsCount = sizeof...(Args);
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

