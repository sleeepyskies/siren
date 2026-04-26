module;

#include <libassert/assert.hpp>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <ranges>
#include <queue>
#include <new>

export module siren.ecs:scheduler;

import :world;

import siren.common;

namespace siren::ecs {

enum class ThreadPolicy {
    AnyThread,
    MainThread,
};

using SystemId      = usize;
using SystemInvoker = std::function<void()>;

template <typename F>
[[nodiscard]] auto system_id(const F& f) noexcept -> SystemId {
    if constexpr (std::is_pointer_v<std::decay_t<F>>) {
        return reinterpret_cast<SystemId>(reinterpret_cast<const void*>(f));
    } else {
        return typeid(std::decay_t<F>).hash_code();
    }
}

struct SystemDescriptor {
    SystemId id;
    std::string name;
    SystemInvoker invoker;
    ThreadPolicy thread_policy{ ThreadPolicy::AnyThread };
    std::vector<SystemId> after;
    std::vector<SystemId> before;
};

export enum class PhaseMode : u8 {
    Once,
    Loop,
};

export using PhaseId = usize;

template <typename Phase>
constexpr auto phase_id() -> PhaseId {
    return typeid(Phase).hash_code();
}

struct PhaseDescriptor {
    PhaseId id;
    std::string name;
    PhaseMode mode{ PhaseMode::Loop };
    std::vector<PhaseId> after;
    std::vector<PhaseId> before;

    std::vector<SystemDescriptor> systems;
    std::vector<SystemId> system_order;
};

// ============================================================================
// == MARK: Misc
// ============================================================================

export template <typename F>
concept IsSystem = (
    std::is_function_v<std::remove_pointer_t<std::remove_cvref_t<F>>> ||
    std::is_convertible_v<F, void(*)()>
) && !std::is_member_function_pointer_v<F>;

export class SystemBuilder {
public:
    SystemBuilder(const PhaseDescriptor& pnode, SystemDescriptor& snode) : m_pnode(pnode), m_snode(snode) { }

    template <typename After>
    auto after(After&& after) -> SystemBuilder& {
        assert_exists<After>();
        m_snode.after.emplace(system_id<After>(after));
        return *this;
    }

    template <typename Before>
    auto before(Before&& before) -> SystemBuilder& {
        assert_exists<Before>();
        m_snode.before.emplace(system_id<Before>(before));
        return *this;
    }

    auto main_thread() -> SystemBuilder& {
        m_snode.thread_policy = ThreadPolicy::MainThread;
        return *this;
    }

    auto any_thread() -> SystemBuilder& {
        m_snode.thread_policy = ThreadPolicy::AnyThread;
        return *this;
    }

private /* Functions */:
    template <typename Other>
    auto assert_exists() -> void {
        auto sid   = system_id<Other>();
        auto found = std::ranges::any_of(
            m_pnode.systems, [sid] (const SystemDescriptor& node) { return node.id == sid; }
        );
        ASSERT(
            found,
            std::format("Cannot reference system {} that has not yet been registered", sid)
        );
    }

private /* State */:
    const PhaseDescriptor& m_pnode;
    SystemDescriptor& m_snode;
};

export class PhaseBuilder {
public:
    explicit PhaseBuilder(PhaseDescriptor& pnode) : m_pnode(pnode) { }

    auto loop() -> PhaseBuilder& {
        m_pnode.mode = PhaseMode::Loop;
        return *this;
    }

    auto once() -> PhaseBuilder& {
        m_pnode.mode = PhaseMode::Once;
        return *this;
    }

    template <typename After>
    auto after() -> PhaseBuilder& {
        m_pnode.after.push_back(phase_id<After>());
        return *this;
    }

    template <typename Before>
    auto before() -> PhaseBuilder& {
        m_pnode.before.push_back(phase_id<Before>());
        return *this;
    }

private:
    PhaseDescriptor& m_pnode;
};

// ============================================================================
// == MARK: Scheduler
// ============================================================================

export class Scheduler {
public:
    explicit Scheduler(const Resolver& resolver) : m_resolver(resolver) { }

    auto build() -> void {
        if (m_built) { return; }

        sort_phases();
        for (const auto node_id : m_order) {
            PhaseId pid = m_graph.node(node_id);
            sort_systems_in_phase(m_phases.at(pid));
        }

        m_built = true;
    }

    template <typename Phase>
    auto add_phase() -> PhaseBuilder {
        ASSERT(!m_built, "Cannot register more phases after scheduler has built.");

        const auto pid = phase_id<Phase>();
        const std::string name{ typename_of<Phase>() };

        ASSERT(!m_phases.contains(pid), std::format("Attempted to register {} more than once.", name));
        log::info("Registering new phase {}", name);

        m_phases.emplace(pid, PhaseDescriptor{ .id = pid, .name = std::move(name) });

        return PhaseBuilder{ m_phases.at(pid) };
    }

    template <typename Phase, IsSystem System>
    auto add_system(System&& system, const std::string& name = "unnamed") -> SystemBuilder {
        ASSERT(!m_built, "Cannot register more systems after scheduler has built.");

        const auto pid   = phase_id<Phase>();
        const auto pname = std::string{ typename_of<Phase>() };
        const auto sid   = system_id(system);
        auto invoker     = make_invoker(std::move(system));

        ASSERT(m_phases.contains(pid), std::format("Attempted to add system for non existent phase {}", pname));
        log::debug("Adding new system {}", name);

        auto& pnode = m_phases.at(pid);
        auto& snode = pnode.systems.emplace_back(
            SystemDescriptor{
                .id = sid,
                .name = std::move(name),
                .invoker = std::move(invoker)
            }
        );

        return SystemBuilder{ pnode, snode };
    }

    template <typename Phase>
    auto run_phase(World& world) -> void {
        const auto pid = phase_id<Phase>();
        auto& phase    = m_phases.at(pid);

        for (SystemId sid : phase.system_order) {
            auto it = std::ranges::find_if(phase.systems, [sid] (auto& s) { return s.id == sid; });

            if (it->thread_policy == ThreadPolicy::MainThread) {
                // do it now
                it->invoker();
            } else {
                // send to thread pool
                auto future = sync::ThreadPool::get().spawn(it->invoker);
                future.wait();
            }
        }
    }

private:
    auto sort_phases() -> void {
        std::unordered_map<PhaseId, DirectedGraph<PhaseId>::Id_t> id_map;

        for (const auto& pid : m_phases | std::views::keys) {
            id_map[pid] = m_graph.insert_node(pid);
        }

        for (const auto& [pid, desc] : m_phases) {
            const auto current_node = id_map.at(pid);
            for (auto after_id : desc.after) {
                m_graph.insert_edge(id_map.at(after_id), current_node);
            }
            for (auto before_id : desc.before) {
                m_graph.insert_edge(current_node, id_map.at(before_id));
            }
        }

        auto sorted = m_graph.topo_sort();
        ASSERT(sorted.has_value(), "Cycle detected in phases!");
        m_order = std::move(*sorted);
    }

    auto sort_systems_in_phase(PhaseDescriptor& phase) -> void {
        DirectedGraph<SystemId> graph;
        std::unordered_map<SystemId, DirectedGraph<SystemId>::Id_t> id_map;

        for (const auto& sys : phase.systems) {
            id_map[sys.id] = graph.insert_node(sys.id);
        }

        for (const auto& sys : phase.systems) {
            const auto current_node = id_map.at(sys.id);
            for (auto after_id : sys.after) {
                graph.insert_edge(id_map.at(after_id), current_node);
            }
            for (auto before_id : sys.before) {
                graph.insert_edge(current_node, id_map.at(before_id));
            }
        }

        const auto sorted = graph.topo_sort();
        ASSERT(sorted.has_value(), std::format("Cycle in phase {} systems!", phase.name));

        phase.system_order.clear();
        for (const auto node_id : *sorted) {
            phase.system_order.push_back(graph.node(node_id));
        }
    }

    template <IsSystem System>
    auto make_invoker(System&& system) -> SystemInvoker {
        using Traits = FunctionTraits<std::decay_t<System>>;
        using Args   = Traits::ArgsPack;

        return [this, system = std::move(system)] mutable -> void {
            [&]<typename... P> (TypePack<P...>) {
                system(m_resolver.resolve<P>()...);
            }(typename ToTypePack<Args>::Type{ });
        };
    }

    const Resolver& m_resolver;
    std::unordered_map<PhaseId, PhaseDescriptor> m_phases;
    DirectedGraph<PhaseId> m_graph;
    std::vector<DirectedGraph<PhaseId>::Id_t> m_order;
    bool m_built = false;
};

} // namespace siren::schedule
