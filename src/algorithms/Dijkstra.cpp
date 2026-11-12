// ===== FILE: src/algorithms/Dijkstra.cpp =====
#include "Dijkstra.h"
#include "../core/MinHeap.h"
#include "../utils/Timer.h"
#include <climits>
#include <unordered_map>
#include <algorithm>
#include <iostream>

PathResult Dijkstra::findPath(const core::Graph& graph, int sourceId, int destId) {
    PathResult result;
    result.algorithmName = "Dijkstra";

    if (!graph.hasNode(sourceId) || !graph.hasNode(destId)) {
        return result;
    }

    Timer timer;

    // Edge case: source == dest
    if (sourceId == destId) {
        result.found = true;
        result.totalDistance = 0;
        result.path = {sourceId};
        result.explorationOrder = {sourceId};
        result.nodesExplored = 1;
        result.executionTimeMs = timer.elapsedMs();
        return result;
    }

    auto allIds = graph.getAllNodeIds();
    std::unordered_map<int, long long> dist;
    std::unordered_map<int, int> parent;
    std::unordered_map<int, bool> visited;

    for (int id : allIds) {
        dist[id] = LLONG_MAX;
        parent[id] = -1;
        visited[id] = false;
    }

    dist[sourceId] = 0;
    MinHeap<HeapNode, std::greater<HeapNode>> minHeap;
    minHeap.push({sourceId, 0});

    while (!minHeap.empty()) {
        HeapNode current = minHeap.top();
        minHeap.pop();

        int u = current.nodeId;

        if (visited[u]) continue;
        visited[u] = true;

        result.nodesExplored++;
        result.explorationOrder.push_back(u);

        // If we reached destination
        if (u == destId) {
            break;
        }

        // Relaxation
        for (const auto& edge : graph.getNeighbors(u)) {
            int v = edge.target;
            long long weight = edge.weight;

            if (!visited[v] && dist[u] != LLONG_MAX && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
                minHeap.push({v, dist[v]});
            }
        }
    }

    result.executionTimeMs = timer.elapsedMs();

    // Check if destination was reached
    if (dist[destId] != LLONG_MAX) {
        result.found = true;
        result.totalDistance = dist[destId];

        // Reconstruct path
        int curr = destId;
        while (curr != -1) {
            result.path.push_back(curr);
            curr = parent[curr];
        }
        std::reverse(result.path.begin(), result.path.end());
    }

    std::cout << "[INFO] Dijkstra finished: found=" << (result.found ? "true" : "false")
              << ", distance=" << result.totalDistance
              << ", nodesExplored=" << result.nodesExplored
              << ", time=" << result.executionTimeMs << "ms" << std::endl;

    return result;
}
