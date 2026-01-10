#pragma once

#include <future>

#include "utilities/spch.hpp"
#include "AssetHandle.hpp"
#include "AssetLoader.hpp"

#include "core/FileSystem.hpp"
#include "core/Locator.hpp"
#include "core/ThreadPool.hpp"


namespace siren::core
{
class AssetServer
{
    using type_t = AssetID::type_t;

    struct AssetContext
    {
        Own<AssetPoolBase> pool;
        Own<AssetLoaderBase> loader;
    };

public:
    template <typename A>
    void register_pool()
    {
        const type_t type_id = AssetID::get_type_id<A>();
        const auto it        = m_context.find(type_id);

        // no context exists yet
        if (it == m_context.end()) {
            m_context.emplace(type_id, AssetContext{ .pool = create_own<AssetPool<A>>() });
            return;
        }

        // context exists but with no pool
        auto& context = it->second;
        if (!context.pool) { context.pool = create_own<AssetPool<A>>(); }
    }

    template <typename A>
    [[nodiscard]]
    AssetPool<A>* get_pool()
    {
        const type_t type_id = AssetID::get_type_id<A>();
        const auto it        = m_context.find(type_id);
        if (it == m_context.end() || !it->second.pool) { return nullptr; }
        return static_cast<AssetPool<A>*>(it->second.pool.get());
    }

    template <typename A>
    void register_loader()
    {
        const type_t type_id = AssetID::get_type_id<A>();
        const auto it        = m_context.find(type_id);

        // no context exists yet
        if (it == m_context.end()) {
            m_context.emplace(type_id, AssetContext{ .loader = create_own<AssetLoader<A>>() });
            return;
        }

        // context exists but with no pool
        auto& context = it->second;
        if (!context.loader) { context.pool = create_own<AssetLoader<A>>(); }
    }

    template <typename A>
    [[nodiscard]]
    AssetHandle<A> load(const Path& path)
    {
        // asset has already been cached
        if (const auto it = m_path_to_id.find(path); it != m_path_to_id.end()) {
            return AssetHandle<A>{
                it->second,
                AssetID::get_type_id<A>()
            };
        }

        const type_t type_id = AssetID::get_type_id<A>();
        const auto it        = m_context.find(type_id);
        if (it == m_context.end() || !it->second.loader) { return nullptr; } // no context or loader

        if (!it->second.pool) { register_pool<A>(); } // no pool, so create one
        AssetPool<A>* pool = static_cast<Own<AssetPool<A>>>(it->second.pool).get();

        // spawn non-blocking loading task
        AssetID asset_id = pool->reserve();

        Locator<ThreadPool>::locate().enqueue(
            [it, path, pool, asset_id]() {
                auto loader = static_cast<AssetLoader<A>*>(it->second.loader.get());
                A* asset    = loader->load(path);
                pool->store(asset_id, asset);
            }
        );

        return AssetHandle<A>{ asset_id, pool };
    }

    template <typename A, typename... Args>
    [[nodiscard]]
    AssetHandle<A> add(Args&&... args)
    {
        const type_t type_id = AssetID::get_type_id<A>();
        const auto it        = m_context.find(type_id);
        if (it == m_context.end()) { return nullptr; } // no context for this asset
        if (!it->second.pool) { return nullptr; }      // cannot store this asset
        auto pool        = static_cast<Own<AssetPool<A>>>(it->second.pool).get();
        const AssetID id = pool->add(std::forward<Args>(args)...);

        return AssetHandle<A>{ id, pool };
    }

private:
    HashMap<Path, AssetID> m_path_to_id{ };
    HashMap<type_t, AssetContext> m_context{ };
};
} // namespace siren::core
