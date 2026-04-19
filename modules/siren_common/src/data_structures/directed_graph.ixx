module;

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <ranges>
#include <algorithm>
#include <optional>
#include <queue>
#include <libassert/assert.hpp>

export module siren.common:directed_graph;

import :typedefs;

namespace siren {

export template <typename Node>
class DirectedGraph {
public:
    using Value_t     = Node;
    using Reference_t = Value_t&;
    using Id_t        = usize;

    DirectedGraph() = default;

    /** @brief Checks if the graph contains any nodes. */
    [[nodiscard]] auto empty() const noexcept -> bool { return m_nodes.empty(); }

    /** @brief Counts the number of nodes in the graph. */
    [[nodiscard]] auto size() const noexcept -> usize { return m_nodes.size(); }

    /** @brief Checks if the graph contains any edges. */
    [[nodiscard]] auto empty_edges() const noexcept -> bool {
        return std::ranges::all_of(m_nodes | std::views::values, [] (const NodeData& node) { return node.in.empty(); });
    }

    /** @brief Counts the number of edges in the graph. */
    [[nodiscard]] auto size_edges() const noexcept -> bool {
        return std::ranges::fold_left(
            m_nodes, usize{ 0 }, [] (usize acc, const NodeData& node) { return acc + node.out; }
        );
    }

    /** @brief Erases all data in the graph. */
    auto clear() noexcept -> void {
        m_nodes.clear();
        m_next = 0;
    }

    /**
     * @brief Adds a new node inplace into the directed graph.
     * @param args Constructor arguments for the node type.
     * @return The id of the newly inserted node.
     */
    template <typename... Args>
    [[nodiscard]] auto emplace_node(Args&&... args) -> Id_t {
        const Id_t id         = next();
        auto [node, inserted] = m_nodes.try_emplace(std::forward<Args>(args)...);
        ASSERT(inserted, "Error encountered when inserting a node into a DirectedGraph");
        return id;
    }

    /**
     * @brief Inserts a new node into the directed graph.
     * @param node The node value to insert.
     * @return The id of the newly inserted node.
     */
    [[nodiscard]] auto insert_node(const Value_t& node) -> Id_t { return emplace_node(node); }

    /**
     * @brief Inserts a new node into the directed graph.
     * @param node The node value to insert.
     * @return The id of the newly inserted node.
     */
    [[nodiscard]] auto insert_node(Value_t&& node) -> Id_t { return emplace_node(std::move(node)); }

    /**
     * @brief Checks if a node exists in the graph.
     * @param id The id to check.
     */
    [[nodiscard]] auto contains_node(const Id_t id) const -> bool { return m_nodes.contains(id); }

    /**
     * @brief Returns the node for an id.
     * @param id The id to fetch the node for.
     */
    [[nodiscard]] auto node(const Id_t id) -> Reference_t { return node_data(id).value; }

    /**
     * @brief Returns the node for an id.
     * @param id The id to fetch the node for.
     */
    [[nodiscard]] auto node(const Id_t id) const -> const Reference_t { return node_data(id).value; }

    /**
     * @brief Erases a node from the graph.
     * @param id The id to remove.
     * @return True if the node was present, false otherwise.
     */
    auto erase_node(const Id_t id) -> bool {
        auto it = m_nodes.find(id);
        if (it == m_nodes.end()) {
            return false;
        }

        for (const auto successor : it->second.out) {
            m_nodes.at(successor).in.erase(id);
        }

        for (const auto predecessor : it->second.in) {
            m_nodes.at(predecessor).out.erase(id);
        }

        m_nodes.erase(it);
        return true;
    }

    /**
     * @brief Creates a new edge in the graph.
     * @param from The starting node.
     * @param to The end node.
     */
    auto insert_edge(const Id_t from, const Id_t to) {
        ASSERT(contains_node(from) && contains_node(to), "Attempted to insert edge for node that does not exist.");
        node_data(from).out.emplace(to);
        node_data(to).in.emplace(from);
    }

    /**
     * @brief Removes an edge from the graph.
     * @param from The starting node.
     * @param to The end node.
     * @return True if the edge existed and was removed, false otherwise.
     */
    auto erase_edge(const Id_t from, const Id_t to) -> bool {
        if (!contains_node(from) || !contains_node(to)) {
            return false;
        }

        auto& out = node_data(from).out;
        auto& in  = node_data(to).in;

        auto out_it = std::ranges::find(out, to);
        if (out_it == out.end()) {
            return false;
        }
        out.erase(out_it);

        auto in_it = std::ranges::find(in, from);
        if (in_it != in.end()) {
            in.erase(in_it);
        }

        return true;
    }

    /**
     * @brief Checks the existence of an edge.
     * @param from The starting node.
     * @param to The end node.
     */
    [[nodiscard]] auto contains_edge(const Id_t from, const Id_t to) noexcept -> bool {
        if (!contains_node(from) || !contains_node(to)) {
            return false;
        }

        auto& out = node_data(from).out;
        auto& in  = node_data(to).in;

        return out.contains(to) && in.contains(from);
    }

    /**
     * @brief Fetches the predecessors of a given node.
     * @param id The node to fetch the predecessors for.
     */
    [[nodiscard]] auto pred(const Id_t id) -> std::unordered_set<Id_t>& { return node_data(id).in; }

    /**
     * @brief Fetches the successors of a given node.
     * @param id The node to fetch the successors for.
     */
    [[nodiscard]] auto succ(const Id_t id) -> std::unordered_set<Id_t>& { return node_data(id).out; }

    /**
     * @brief Topologically sorts the graph.
     * @return An optional sorted vector of id's in the order they should occur. Nothing if there are cycles.
     */
    auto topo_sort() const -> std::optional<std::vector<Id_t>> {
        std::vector<Id_t> result;
        std::unordered_map<Id_t, usize> indegree;
        std::queue<Id_t> queue;

        // build queue and count indegree
        for (const auto& [id, node] : m_nodes) {
            const usize indeg = node.in.size();
            indegree[id]      = indeg;
            if (indeg == 0) {
                queue.emplace(id);
            }
        }

        // process nodes
        while (!queue.empty()) {
            const auto u = queue.front();
            queue.pop();
            result.push_back(u);

            for (const auto& v : node_data(u).out) {
                --indegree[v];
                if (indegree[v] == 0) {
                    queue.emplace(v);
                }
            }
        }

        // check cycles
        if (result.size() != size()) {
            return std::nullopt;
        }

        return result;
    }

private:
    struct NodeData {
        Value_t value;
        std::unordered_set<Id_t> out;
        std::unordered_set<Id_t> in;
    };

    auto next() -> Id_t { return m_next++; }

    auto node_data(const Id_t id) -> NodeData& {
        ASSERT(contains_node(id), "Node does not exist in graph");
        return m_nodes.at(id);
    }

    auto node_data(const Id_t id) const -> const NodeData& {
        ASSERT(contains_node(id), "Node does not exist in graph");
        return m_nodes.at(id);
    }

    Id_t m_next{ 0 };
    std::unordered_map<Id_t, NodeData> m_nodes;
};

} // namespace siren
