// ===== FILE: src/core/Graph.h =====
#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>

/**
 * @file Graph.h
 * @brief Adjacency list graph representation with hash map ID-to-index mapping.
 *
 * CORE DATA STRUCTURE (FODS Requirement):
 * Hand-implemented adjacency list (`std::vector<std::vector<Edge>>`) paired with
 * a vertex list and a hash map wrapper (`std::unordered_map<int, int>`) for O(1) node lookup.
 *
 * Time Complexities:
 *   - addNode       : O(1) amortized
 *   - addEdge       : O(1)
 *   - getNode       : O(1) via hash map
 *   - getNeighbors  : O(1)
 * Space Complexity  : O(V + E)
 */

namespace core {

struct Edge {
    int target;
    long long weight;
};

struct Node {
    int id;
    double x;
    double y;
    std::string name;
};

class Graph {
public:
    Graph() = default;

    /**
     * @brief Add a node to the graph. Time: O(1).
     */
    void addNode(int id, double x, double y, const std::string& name);

    /**
     * @brief Add a directed edge between sourceId and targetId. Time: O(1).
     */
    void addEdge(int sourceId, int targetId, long long weight);

    /**
     * @brief Get outgoing neighbors for a given node ID. Time: O(1).
     */
    const std::vector<Edge>& getNeighbors(int nodeId) const;

    /**
     * @brief Get Node details by ID. Time: O(1).
     */
    const Node& getNode(int nodeId) const;

    /**
     * @brief Check if node exists. Time: O(1).
     */
    bool hasNode(int id) const;

    /**
     * @brief Get internal vector index from node ID. Time: O(1).
     */
    int getInternalIndex(int nodeId) const;

    /**
     * @brief Get node ID from internal vector index. Time: O(1).
     */
    int getNodeIdFromIndex(int index) const;

    /**
     * @brief Get total node count. Time: O(1).
     */
    int nodeCount() const;
    int getNodeCount() const { return nodeCount(); }

    /**
     * @brief Get total edge count. Time: O(1).
     */
    int edgeCount() const;
    int getEdgeCount() const { return edgeCount(); }

    /**
     * @brief Get all node IDs in the graph. Time: O(V).
     */
    std::vector<int> getAllNodeIds() const;

    /**
     * @brief Clear all graph data.
     */
    void clear();

private:
    std::vector<Node> m_nodes;
    std::vector<std::vector<Edge>> m_adjList;
    std::unordered_map<int, int> m_idToIndex;
    int m_edgeCount = 0;
};

} // namespace core
