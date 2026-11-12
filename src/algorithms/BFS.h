// ===== FILE: src/algorithms/BFS.h =====
#pragma once
#include "PathResult.h"
#include "../core/Graph.h"

/**
 * @file BFS.h
 * @brief Breadth-First Search (BFS) for unweighted shortest path / cross-verification.
 *
 * @section complexity Time & Space Complexity:
 * - Time Complexity : O(V + E)
 * - Space Complexity: O(V) for queue, visited, and parent tracking
 */
class BFS {
public:
    /**
     * @brief Find shortest path between sourceId and destId using BFS.
     */
    static PathResult findPath(const core::Graph& graph, int sourceId, int destId);
};
