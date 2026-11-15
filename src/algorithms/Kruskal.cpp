#include "Kruskal.h"
#include <algorithm>
#include <unordered_map>

namespace algorithms {
namespace {

struct CandidateEdge {
    int source;
    int target;
    long long weight;
};

class DisjointSet {
public:
    explicit DisjointSet(const std::vector<int>& nodeIds) {
        for (int id : nodeIds) {
            m_parent[id] = id;
            m_rank[id] = 0;
        }
    }

    int find(int id) {
        if (m_parent[id] != id) {
            m_parent[id] = find(m_parent[id]);
        }
        return m_parent[id];
    }

    bool unite(int first, int second) {
        int firstRoot = find(first);
        int secondRoot = find(second);
        if (firstRoot == secondRoot) return false;

        if (m_rank[firstRoot] < m_rank[secondRoot]) {
            std::swap(firstRoot, secondRoot);
        }
        m_parent[secondRoot] = firstRoot;
        if (m_rank[firstRoot] == m_rank[secondRoot]) {
            ++m_rank[firstRoot];
        }
        return true;
    }

private:
    std::unordered_map<int, int> m_parent;
    std::unordered_map<int, int> m_rank;
};

} // namespace

MstResult kruskalMst(const core::Graph& graph) {
    const auto nodeIds = graph.getAllNodeIds();
    std::vector<CandidateEdge> candidates;

    for (int source : nodeIds) {
        for (const auto& edge : graph.getNeighbors(source)) {
            // CSVParser stores each undirected road in both directions.
            if (source < edge.target) {
                candidates.push_back({source, edge.target, edge.weight});
            }
        }
    }

    std::sort(candidates.begin(), candidates.end(), [](const CandidateEdge& left,
                                                       const CandidateEdge& right) {
        if (left.weight != right.weight) return left.weight < right.weight;
        if (left.source != right.source) return left.source < right.source;
        return left.target < right.target;
    });

    DisjointSet sets(nodeIds);
    MstResult result;
    result.components = static_cast<int>(nodeIds.size());

    for (const auto& candidate : candidates) {
        if (sets.unite(candidate.source, candidate.target)) {
            result.edges.emplace_back(candidate.source, candidate.target);
            result.totalWeight += candidate.weight;
            --result.components;
        }
    }

    return result;
}

} // namespace algorithms
