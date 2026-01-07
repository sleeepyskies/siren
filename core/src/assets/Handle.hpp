#pragma once

#include <atomic>


namespace siren::core
{
struct RawHandle
{
    using index_t = u32;
    using gen_t   = u16;
    using meta_t  = u16;

    union
    {
        u64 handle = 0; ///< The full packed handle for fast comparisons.
        struct
        {
            u32 index : 32;      ///< ID used for indexing.
            u16 generation : 16; ///< The generation of this slot.
            u16 meta : 16;       ///< Any additional meta data, flags etc.
        };
    };

    RawHandle() = default;
    RawHandle(
        const index_t index,
        const gen_t gen,
        const meta_t meta = 0
    ) : index(index), generation(gen), meta(meta) { }

    [[nodiscard]]
    bool is_valid() const { return handle != 0; }
    [[nodiscard]]
    explicit operator bool() const { return is_valid(); }

    [[nodiscard]]
    bool operator==(const RawHandle& other) const { return handle == other.handle; }
    [[nodiscard]]
    auto operator<=>(const RawHandle& other) const { return handle <=> other.handle; }
};

enum class LoadStatus
{
    Unloaded,
    Loading,
    Loaded,
};

template <typename A>
class AssetPool
{
public:
    using gen_t   = RawHandle::gen_t;
    using index_t = RawHandle::index_t;

private:
    using refc_t = u32;

    struct PoolEntry
    {
        Own<A> asset;
        gen_t generation;
        std::atomic<refc_t> ref_count;
        LoadStatus load_staus = LoadStatus::Unloaded;

        // @formatter:off
        void kill() { asset.reset(nullptr); generation++; ref_count = 0; load_staus = LoadStatus::Unloaded; }
        // @formatter:on
    };

public:
    [[nodiscard]]
    RawHandle reserve()
    {
        index_t idx;

        if (!m_free_list.empty()) {
            idx = m_free_list.back();
            m_free_list.pop_back();
        } else {
            idx = (index_t)m_storage.size();
            m_storage.emplace_back();
        }

        PoolEntry& entry = m_storage[idx];
        entry.asset      = nullptr;
        entry.ref_count.store(0);
        entry.load_staus = LoadStatus::Loading;

        return RawHandle{ idx, entry.generation };
    }

    bool store(const RawHandle handle, const A* data)
    {
        if (!is_valid_handle(handle) || !data) { return false; }
        PoolEntry& entry = m_storage[handle.index];
        if (handle.generation != entry.generation) { return false; }

        entry.asset      = CreateOwn<A>(data);
        entry.load_staus = LoadStatus::Loaded;
        return true;
    }

    [[nodiscard]]
    A* get(const RawHandle handle)
    {
        if (!is_valid_handle(handle)) { return nullptr; }
        PoolEntry& entry = m_storage[handle.index];
        if (entry.generation != handle.generation) { return nullptr; }

        return entry.asset.get();
    }

    void inc_ref(const RawHandle handle)
    {
        if (!is_valid_handle(handle)) { return; }
        PoolEntry& entry = m_storage[handle.index];
        if (entry.generation != handle.generation) { return; }
        entry.ref_count.fetch_add(1);
    }

    void dec_ref(const RawHandle handle)
    {
        if (!is_valid_handle(handle)) { return; }
        PoolEntry& entry = m_storage[handle.index];
        if (entry.generation != handle.generation) { return; }

        if (entry.ref_count.fetch_sub().load(std::memory_order_acq_rel) == 0) {
            m_free_list.emplace_back(handle.index);
            entry.kill();
        }
    }

    void force_delete(const RawHandle handle)
    {
        if (!is_valid_handle(handle)) { return; }
        PoolEntry& entry = m_storage[handle.index];
        if (entry.generation != handle.generation) { return; }
        m_free_list.emplace_back(handle.index);
        entry.kill();
    }

private:
    bool is_valid_handle(const RawHandle handle) const { return handle.index < m_storage.size() && handle.is_valid(); }

    Vector<PoolEntry> m_storage;
    Vector<index_t> m_free_list;
};

template <typename A>
class Handle
{
public:
    Handle() : m_handle(), m_pool(nullptr) { }

    Handle(const RawHandle& handle, AssetPool<A>* pool) : m_handle(handle), m_pool(pool)
    {
        if (pool) { pool->inc_ref(m_handle); }
    }

    Handle(const Handle& other) : m_handle(other.m_handle), m_pool(other.m_pool)
    {
        if (m_pool) { m_pool->inc_ref(m_handle); }
    }

    Handle& operator=(const Handle& other)
    {
        if (this == &other) { return *this; }
        if (m_pool) { m_pool->dec_ref(m_handle); }
        m_pool   = other.m_pool;
        m_handle = other.m_handle;
        if (m_pool) { m_pool->inc_ref(m_handle); }
        return *this;
    }

    Handle(Handle&& other) noexcept : m_handle(other.m_handle), m_pool(other.m_pool)
    {
        other.m_pool   = nullptr;
        other.m_handle = RawHandle{ };
    }

    Handle& operator=(Handle&& other) noexcept
    {
        if (this == &other) { return *this; }
        if (m_pool) { m_pool->dec_ref(m_handle); }
        m_pool         = other.m_pool;
        m_handle       = other.m_handle;
        other.m_pool   = nullptr;
        other.m_handle = RawHandle{ };
        return *this;
    }

    ~Handle() { if (m_pool) { m_pool->dec_ref(m_handle); } }

    [[nodiscard]]
    RawHandle raw() const { return m_handle; }

    [[nodiscard]]
    A* get() const { return m_pool ? m_pool->get(m_handle) : nullptr; }
    [[nodiscard]]
    A* operator->() const { return get(); }
    [[nodiscard]]
    A& operator*() const { return *get(); }
    [[nodiscard]]
    explicit operator bool() const { return get() != nullptr; }
    [[nodiscard]]
    bool operator==(const Handle& other) const { return m_pool == other.m_pool && m_handle == other.m_handle; }

private:
    RawHandle m_handle;
    AssetPool<A>* m_pool;
};

class AssetServer { };
}
