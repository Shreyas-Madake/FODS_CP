// ===== FILE: src/algorithms/AStar.h =====
#pragma once
#include "PathResult.h"
#include "../core/Graph.h"

/**
 * @file AStar.h
 * @brief A* Search Algorithm using Euclidean distance heuristic and custom MinHeap.
 *
 * @section complexity Time & Space Complexity:
 * - Time Complexity : O((V + E) log V) worst case, significantly faster in practice with good heuristic
 * - Space Complexity: O(V) for gScore, fScore, visited, parent arrays and heap storage
 *
 * @section heuristic Heuristic:
 * Uses straight-line Euclidean distance between node coordinates `sqrt((x2-x1)^2 + (y2-y1)^2)`.
 * Guaranteed to be admissible and consistent.
 */
class AStar {
public:
    /**
     * @brief Find shortest path between sourceId and destId using A* search.
     */
    static PathResult findPath(const core::Graph& graph, int sourceId, int destId);
};
