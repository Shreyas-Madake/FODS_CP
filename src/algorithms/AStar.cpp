// ===== FILE: src/algorithms/AStar.cpp =====
#include "AStar.h"
#include "../core/MinHeap.h"
#include "../utils/Timer.h"
#include <climits>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <iostream>

namespace {
    double computeEuclideanHeuristic(const core::Node& a, const core::Node& b) {
        double dx = a.x - b.x;
        double dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy) * 111.0; // Rough conversion factor degrees to km (~111 km per degree)
    }
}

PathResult AStar::findPath(const core::Graph& graph, int sourceId, int destId) {
    PathResult result;
    result.algorithmName = "A* Search";

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

    const auto& destNode = graph.getNode(destId);
    auto allIds = graph.getAllNodeIds();

    std::unordered_map<int, long long> gScore;
    std::unordered_map<int, long long> fScore;
    std::unordered_map<int, int> parent;
    std::unordered_map<int, bool> visited;

    for (int id : allIds) {
        gScore[id] = LLONG_MAX;
        fScore[id] = LLONG_MAX;
        parent[id] = -1;
        visited[id] = false;
    }

    gScore[sourceId] = 0;
    double hStart = computeEuclideanHeuristic(graph.getNode(sourceId), destNode);
    fScore[sourceId] = static_cast<long long>(hStart);

    MinHeap<HeapNode, std::greater<HeapNode>> minHeap;
    minHeap.push({sourceId, fScore[sourceId]});

    while (!minHeap.empty()) {
        HeapNode current = minHeap.top();
        minHeap.pop();

        int u = current.nodeId;

        if (visited[u]) continue;
        visited[u] = true;

        result.nodesExplored++;
        result.explorationOrder.push_back(u);

        if (u == destId) {
            break;
        }

        for (const auto& edge : graph.getNeighbors(u)) {
            int v = edge.target;
            long long weight = edge.weight;

            if (visited[v]) continue;

            long long tentativeG = gScore[u] + weight;
            if (tentativeG < gScore[v]) {
                parent[v] = u;
                gScore[v] = tentativeG;
                double h = computeEuclideanHeuristic(graph.getNode(v), destNode);
                fScore[v] = tentativeG + static_cast<long long>(h);
                minHeap.push({v, fScore[v]});
            }
        }
    }

    result.executionTimeMs = timer.elapsedMs();

    if (gScore[destId] != LLONG_MAX) {
        result.found = true;
        result.totalDistance = gScore[destId];

        int curr = destId;
        while (curr != -1) {
            result.path.push_back(curr);
            curr = parent[curr];
        }
        std::reverse(result.path.begin(), result.path.end());
    }

    std::cout << "[INFO] A* finished: found=" << (result.found ? "true" : "false")
              << ", distance=" << result.totalDistance
              << ", nodesExplored=" << result.nodesExplored
              << ", time=" << result.executionTimeMs << "ms" << std::endl;

    return result;
}
