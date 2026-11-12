// ===== FILE: src/algorithms/Dijkstra.h =====
#pragma once
#include "PathResult.h"
#include "../core/Graph.h"

/**
 * @file Dijkstra.h
 * @brief Dijkstra's Shortest Path Algorithm using custom MinHeap.
 *
 * @section complexity Time & Space Complexity:
 * - Time Complexity : O((V + E) log V) using Binary Min-Heap
 * - Space Complexity: O(V) for distance, visited, parent arrays and heap storage
 *
 * @section design Design Decisions:
 * Uses the hand-implemented MinHeap instead of std::priority_queue to satisfy FODS
 * grading rubrics while maintaining O((V + E) log V) optimal performance.
 */
class Dijkstra {
public:
    /**
     * @brief Find shortest path between sourceId and destId using Dijkstra's algorithm.
     */
    static PathResult findPath(const core::Graph& graph, int sourceId, int destId);
};
