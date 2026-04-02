module;

#include <algorithm>
#include <vector>
#include <optional>

export module siren.schedule.execution_graph;

import siren.common;

namespace siren::schedule {

export template <typename T>
class DiGraph {
public:
    struct Node {
        T value;
        std::vector<usize> outgoing;
    };

    auto add_node(const T& value) -> usize {
        nodes.push_back(Node{ value, { } });
        return nodes.size() - 1;
    }

    auto add_edge(const usize from, const usize to) -> void {
        nodes[from].outgoing.emplace_back(to);
    }

    auto remove_edge(const usize from, const usize to) -> void {
        auto& outgoing = nodes[from].outgoing;
        auto ret       = std::ranges::remove(outgoing, to);
        outgoing.erase(ret.begin(), ret.end());
    }

    auto begin();

private:
    std::vector<Node> nodes;
};

export template <typename T>
class ExecutionGraph {
public:
    struct Node {
        T value;
    };

private:
};

auto x() {
    ExecutionGraph<int> graph;

    graph.emplace(0, 1, 9, 8, 4, 3);
    graph.emplace(2).after(8);
    graph.emplace(3).before(2, 8, 4, 3).after(9, 0, 1);

    for (const auto& node : graph) {
        std::prinln(node);
    }
}

} // namespace siren::schedule

