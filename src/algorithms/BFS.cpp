// ===== FILE: src/algorithms/BFS.cpp =====
#include "BFS.h"
#include "../utils/Timer.h"
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>

PathResult BFS::findPath(const core::Graph& graph, int sourceId, int destId) {
    PathResult result;
    result.algorithmName = "BFS";

    if (!graph.hasNode(sourceId) || !graph.hasNode(destId)) {
        return result;
    }

    Timer timer;

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
    std::unordered_map<int, bool> visited;
    std::unordered_map<int, int> parent;

    for (int id : allIds) {
        visited[id] = false;
        parent[id] = -1;
    }

    std::queue<int> q;
    q.push(sourceId);
    visited[sourceId] = true;

    bool reached = false;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        result.nodesExplored++;
        result.explorationOrder.push_back(u);

        if (u == destId) {
            reached = true;
            break;
        }

        for (const auto& edge : graph.getNeighbors(u)) {
            int v = edge.target;
            if (!visited[v]) {
                visited[v] = true;
                parent[v] = u;
                q.push(v);
            }
        }
    }

    result.executionTimeMs = timer.elapsedMs();

    if (reached) {
        result.found = true;

        // Reconstruct path and compute total weight along path
        int curr = destId;
        long long totalDist = 0;
        while (curr != -1) {
            result.path.push_back(curr);
            int p = parent[curr];
            if (p != -1) {
                // Find edge weight from p to curr
                for (const auto& edge : graph.getNeighbors(p)) {
                    if (edge.target == curr) {
                        totalDist += edge.weight;
                        break;
                    }
                }
            }
            curr = p;
        }
        std::reverse(result.path.begin(), result.path.end());
        result.totalDistance = totalDist;
    }

    std::cout << "[INFO] BFS finished: found=" << (result.found ? "true" : "false")
              << ", distance=" << result.totalDistance
              << ", nodesExplored=" << result.nodesExplored
              << ", time=" << result.executionTimeMs << "ms" << std::endl;

    return result;
}
