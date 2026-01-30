#pragma once

#include "Asset.hpp"
#include "AssetPath.hpp"
#include "core/Locator.hpp"
#include "core/TaskPool.hpp"
#include "loaders/AssetLoader.hpp"


namespace siren::core
{

/// @brief Represents the loading status of an asset.
enum class LoadStatus {
    NotLoaded, ///< @brief The asset is not loaded, and loading hasn't begun.
    Loading,   ///< @brief The asset is currently being loaded by an AssetLoader.
    Loaded,    ///< @brief The asset was successfully loaded.
    Fail,      ///< @brief There was an error while loading the asset.
};

/**
 * @brief Simple state machine class for managing LoadStatus.
 */
class LoadState {
public:
    [[nodiscard]]
    constexpr auto get_main() const noexcept -> LoadStatus { return m_main; }
    constexpr auto set_main(const LoadStatus status) noexcept -> void {
        if (m_main == LoadStatus::Fail || m_dependencies == LoadStatus::Fail) { return; }
        if (status == LoadStatus::NotLoaded) { return; }
        if (status == LoadStatus::Fail) { m_dependencies = status; }
        m_main = status;
    }

    [[nodiscard]]
    constexpr auto get_deps() const noexcept -> LoadStatus { return m_dependencies; }
    constexpr auto set_deps(const LoadStatus status) noexcept -> void {
        if (m_main == LoadStatus::Fail || m_dependencies == LoadStatus::Fail) { return; }
        if (status == LoadStatus::NotLoaded) { return; }
        if (status == LoadStatus::Fail) { m_main = status; }
        m_dependencies = status;
    }

    [[nodiscard]]
    constexpr auto is_ready() const noexcept -> bool {
        return m_main == m_dependencies && m_main == LoadStatus::Loaded;
    }

    [[nodiscard]]
    constexpr auto failed() const noexcept -> bool {
        return m_main == LoadStatus::Fail || m_dependencies == LoadStatus::Fail;
    }

private:
    LoadStatus m_main         = LoadStatus::NotLoaded;
    LoadStatus m_dependencies = LoadStatus::NotLoaded;
};

class AssetServer {
    using TypeID = AssetID::TypeID;

    struct AssetInfo {
        /// @brief The main asset.
        WeakHandle weak_handle;
        /// @brief The load state of this asset and its dependencies.
        LoadState load_state;
        /// @brief Any labeled (aka embedded) assets of the main asset.
        HashMap<String, WeakHandle> labeled_deps;
        /// @brief Any external (aka external files) dependencies of this main asset.
        HashSet<WeakHandle> dependencies;
        /// @brief All assets that are waiting for this asset to finish loading.
        HashSet<WeakHandle> dependents;
    };

    struct Loaders {
        Vector<Own<AssetLoader>> loaders;            ///< @brief Main loader storage.
        HashMap<String, AssetLoader*> ext_to_loader; ///< @brief Cached loaders based on their accepted file extensions.
    };

    struct AssetServerData {
        /// @brief General data on assets (dep tree, load status).
        RwLock<HashMap<HashedString, AssetInfo>> asset_infos;
        /// @brief Main storage for asset data.
        RwLock<HashMap<TypeID, Own<AssetPoolBase>>> storage{ };
        /// @brief All active loaders.
        Loaders loaders;
    };

public:
    template <IsAsset A>
    [[nodiscard]]
    auto get(const StrongHandle<A> handle) -> Option<A&> {
        if (!handle.is_valid() || is_loaded_with_dependencies(handle)) { return None; }

        const auto storage = m_data.storage.read();
        const auto it      = storage->find(AssetID::get_type_id<A>());
        if (it == storage->end()) { return None; }

        return Some(it->second.get().get(handle.id()));
    }

    /// @brief Will load an asset from disk as well as all of its dependencies.
    template <IsAsset A>
    [[nodiscard]]
    auto load(const AssetPath& path) -> StrongHandle<A>;

    /// @brief Directly adds the provided asset into storage, if a pool exists
    /// for its type.
    template <IsAsset A>
    [[nodiscard]]
    auto add(Own<A>&& asset) -> StrongHandle<A> {
        auto storage = m_data.storage.write();

        const TypeID type_id = AssetID::get_type_id<A>();
        const auto it        = storage->find(type_id);
        if (it == storage->end()) {
            err("Could not find an appropriate asset pool for type {}", TypeName<A>::value());
            return StrongHandle<A>::invalid();
        }

        auto pool        = static_cast<AssetPool<A>*>(it->second.get());
        const AssetID id = pool->add(std::forward<Own<A>>(asset));
        return StrongHandle<A>{ id, pool };
    }

    /// @brief Shallow checks if this asset is loaded. State of dependencies is ignored.
    template <IsAsset A>
    auto is_loaded(const StrongHandle<A>& handle) -> bool {
        const auto asset_info = m_data.asset_infos.read();

        const auto it = asset_info->find(AssetID::get_type_id<A>());
        if (it == asset_info->end()) { return false; }

        return it->second.load_status == LoadStatus::Loaded;
    }

    template <IsAsset A>
    auto is_loaded_with_dependencies(const StrongHandle<A>& handle) -> bool {
        const auto asset_info = m_data.asset_infos.read();

        const auto it = asset_info->find(AssetID::get_type_id<A>());
        if (it == asset_info->end()) { return false; }

        // main asset isn't loaded
        if (it->second.load_status != LoadStatus::Loaded) { return false; }

        // some deps havent finshed loading
        return it->second.load_status_dependencies == LoadStatus::Loaded;
    }

    /// @brief Registers an asset type with the server. No assets of this
    /// type can be loaded or stored until registered.
    template <IsAsset A>
    auto register_asset() -> void {
        const TypeID type_id = AssetID::get_type_id<A>();
        auto storage         = m_data.storage.write();
        if (storage->contains(type_id)) { return; }
        storage->emplace(create_own<AssetPool<A>>());
    }

    /// @brief Registers a loader with the asset server. No assets can be
    /// loaded from disk until an appropriate loader has been registered.
    auto register_loader(Own<AssetLoader> loader) -> void {
        auto& loaders         = m_data.loaders;
        const auto loader_ptr = loaders.loaders.emplace_back(std::move(loader)).get();
        for (const auto& ext : loader_ptr->extensions()) { loaders.ext_to_loader.insert({ String(ext), loader_ptr }); }
    }

private:
    friend class LoadContext; // Need access to create the dependency tree.

    AssetServerData m_data; ///< @brief The underlying data of the AssetServer.
};

/**
 * @brief LoadContext serves as the AssetServer API for AssetLoaders. It handles registering sub assets, as well as
 * tracking asset dependencies.
 */
class LoadContext {
public:
    LoadContext(AssetServer& server, const AssetPath& path, const WeakHandle& handle)
        : m_server(server), m_handle(handle) { }

    template <IsAsset A>
    [[nodiscard]]
    auto add_labeled_asset(const String& label, Own<A>&& asset) -> StrongHandle<A> {
        const StrongHandle<A> handle = m_server.add<A>(std::move(asset));

        // the asset_info should exist already, if it doesn't please crash, something went wrong :D
        auto asset_infos = m_server.m_data.asset_infos.write();
        auto& asset_info = asset_infos->at(path().hash());

        // some issue with adding the new labeled asset
        if (!handle.is_valid()) {
            asset_info.load_state.set_main(LoadStatus::Fail);
            poison_dependents(m_handle, *asset_infos);
            return handle;
        }

        asset_info.labeled_deps.emplace(label, handle.as_weak());
        return handle;
    }

    template <IsAsset A>
    [[nodiscard]]
    auto load_external_asset(const AssetPath& asset_path) -> StrongHandle<A> {
        const StrongHandle<A> handle = m_server.load<A>(asset_path);

        // the asset_info should exist already, if it doesn't please crash, something went wrong :D
        auto asset_infos          = m_server.m_data.asset_infos.write();
        auto& main_asset_info     = asset_infos->at(path().hash());
        auto& external_asset_info = asset_infos->at(asset_path.hash());

        // some issue with adding the new external asset
        if (!handle.is_valid()) {
            external_asset_info.load_state.set_main(LoadStatus::Fail);
            poison_dependents(external_asset_info.weak_handle, *asset_infos);
            return handle;
        }

        main_asset_info.dependencies.emplace(handle.as_weak());
        external_asset_info.dependents.emplace(m_handle);
        return handle;
    }

    template <IsAsset A>
    auto finish(Own<A>&& asset) -> void {
        auto asset_infos = m_server.m_data.asset_infos.write();
        auto storage     = m_server.m_data.storage.write();

        // the asset_info should exist already, if it doesn't please crash, something went wrong :D
        auto& asset_info = asset_infos->at(path().hash());
        asset_info.load_state.set_main(LoadStatus::Loaded);

        // if we have no pending sub assets, work up the tree.
        notify_dependents(m_handle, *asset_infos);

        auto& pool = *dynamic_cast<AssetPool<A>*>(storage->at(AssetID::get_type_id<A>()).get());
        pool.store(m_handle.id(), std::move(asset));
    }

    [[nodiscard]]
    constexpr auto path() const noexcept -> AssetPath { return m_handle.path(); }

private:
    auto notify_dependents(
        const WeakHandle& handle,
        HashMap<HashedString, AssetServer::AssetInfo>& asset_infos
    ) const -> void {
        auto& asset_info = asset_infos.at(handle.path().hash());

        // still waiting, do not notify parents yet
        if (!asset_info.dependencies.empty()) { return; }

        asset_info.load_state.set_main(LoadStatus::Loaded);
        asset_info.load_state.set_deps(LoadStatus::Loaded);

        // totally done, work up the tree
        for (auto& parent : asset_info.dependents) {
            auto& parent_info = asset_infos.at(parent.path().hash());
            parent_info.dependencies.erase(handle);
            notify_dependents(parent, asset_infos);
        }
    }

    auto poison_dependents(
        const WeakHandle& handle,
        HashMap<HashedString, AssetServer::AssetInfo>& asset_infos
    ) const -> void {
        auto& asset_info = asset_infos.at(handle.path().hash());

        asset_info.load_state.set_main(LoadStatus::Fail);
        asset_info.load_state.set_deps(LoadStatus::Fail);

        for (auto& parent : asset_info.dependents) {
            poison_dependents(parent, asset_infos);
        }

        // do not load again, as we can cache the failed load,
        // so we don't try to load and fail again
    }

    AssetServer& m_server; ///< @brief The asset server to load into.
    WeakHandle m_handle;   ///< @brief The handle to the main asset.
};

template <IsAsset A>
[[nodiscard]]
auto AssetServer::load(const AssetPath& path) -> StrongHandle<A> {
    auto storage     = m_data.storage.write();
    auto asset_infos = m_data.asset_infos.write();

    const auto storage_it = storage->find(AssetID::get_type_id<A>());
    // cannot store this type
    if (storage_it == storage->end()) { return StrongHandle<A>::invalid(); }
    auto pool = static_cast<AssetPool<A>*>(storage_it->second.get());

    // asset has already been cached
    const auto asset_info_it = asset_infos->find(path.hash());
    if (asset_info_it != asset_infos->end()) {
        // this is a labeled sub asset
        const auto label = path.label();
        if (label.is_some()) {
            const auto label_it = asset_info_it->second.labeled_deps.find(label.unwrap());
            if (label_it != asset_info_it->second.labeled_deps.end()) {
                return StrongHandle<A>::from_weak(label_it->second);
            }
        }
        // this is the main asset
        return StrongHandle<A>::from_weak(asset_info_it->second);
    }

    // check if loader exists
    const auto loader_it = m_data.loaders.ext_to_loader.find(path.extension());
    if (loader_it == m_data.loaders.ext_to_loader.end()) { return StrongHandle<A>::invalid(); }
    auto loader = loader_it->second;

    // spawn non-blocking loading task
    AssetID asset_id = pool->reserve();
    const WeakHandle weak_handle{ asset_id, pool };
    Locator<TaskPool>::locate().spawn(
        [this, path, loader, weak_handle] { loader->load(LoadContext{ *this, path, weak_handle }, nullptr); }
    );

    // init AssetInfo state
    asset_infos->emplace(
        path.hash(),
        AssetInfo{
            .weak_handle = weak_handle,
            .load_state = { },
            .labeled_deps = { },
            .dependencies = { },
            .dependents = { },
        }
    );

    return StrongHandle<A>::from_weak(weak_handle);
}

} // namespace siren::core
