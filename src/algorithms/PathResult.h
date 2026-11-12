// ===== FILE: src/algorithms/PathResult.h =====
#pragma once
#include <vector>
#include <string>

/**
 * @file PathResult.h
 * @brief Unified return structure for all pathfinding algorithms.
 *
 * Allows side-by-side comparison of Dijkstra, A*, and BFS across execution time,
 * nodes explored, and total distance.
 */
struct PathResult {
    bool found = false;
    long long totalDistance = 0;
    std::vector<int> path;              // Ordered list of node IDs from source to dest
    int nodesExplored = 0;
    double executionTimeMs = 0.0;
    std::string algorithmName;

    // Animation support: ordered list of node IDs in the order they were popped/explored
    std::vector<int> explorationOrder;
};
