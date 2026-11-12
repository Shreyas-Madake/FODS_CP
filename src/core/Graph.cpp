// ===== FILE: src/core/Graph.cpp =====
#include "Graph.h"
#include <stdexcept>

namespace core {

void Graph::addNode(int id, double x, double y, const std::string& name) {
    if (m_idToIndex.find(id) != m_idToIndex.end()) {
        return; // Node already exists
    }
    int index = static_cast<int>(m_nodes.size());
    m_nodes.push_back({id, x, y, name});
    m_adjList.push_back({});
    m_idToIndex[id] = index;
}

void Graph::addEdge(int sourceId, int targetId, long long weight) {
    if (!hasNode(sourceId) || !hasNode(targetId)) {
        return; // Ignore edges with invalid nodes
    }
    int uIndex = m_idToIndex[sourceId];
    int vIndex = m_idToIndex[targetId];

    // Add directed edge (or undirected by calling twice, we add directed here)
    m_adjList[uIndex].push_back({targetId, weight});
    m_edgeCount++;
}

const std::vector<Edge>& Graph::getNeighbors(int nodeId) const {
    int idx = getInternalIndex(nodeId);
    return m_adjList[idx];
}

const Node& Graph::getNode(int nodeId) const {
    int idx = getInternalIndex(nodeId);
    return m_nodes[idx];
}

bool Graph::hasNode(int id) const {
    return m_idToIndex.find(id) != m_idToIndex.end();
}

int Graph::getInternalIndex(int nodeId) const {
    auto it = m_idToIndex.find(nodeId);
    if (it == m_idToIndex.end()) {
        throw std::out_of_range("Graph::getInternalIndex — Node ID " + std::to_string(nodeId) + " not found.");
    }
    return it->second;
}

int Graph::getNodeIdFromIndex(int index) const {
    if (index < 0 || static_cast<size_t>(index) >= m_nodes.size()) {
        throw std::out_of_range("Graph::getNodeIdFromIndex — Index out of bounds.");
    }
    return m_nodes[index].id;
}

int Graph::nodeCount() const {
    return static_cast<int>(m_nodes.size());
}

int Graph::edgeCount() const {
    return m_edgeCount;
}

std::vector<int> Graph::getAllNodeIds() const {
    std::vector<int> ids;
    ids.reserve(m_nodes.size());
    for (const auto& node : m_nodes) {
        ids.push_back(node.id);
    }
    return ids;
}

void Graph::clear() {
    m_nodes.clear();
    m_adjList.clear();
    m_idToIndex.clear();
    m_edgeCount = 0;
}

} // namespace core
