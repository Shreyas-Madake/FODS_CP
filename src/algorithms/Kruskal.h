#pragma once

#include "../core/Graph.h"
#include <utility>
#include <vector>

namespace algorithms {

struct MstResult {
    std::vector<std::pair<int, int>> edges;
    long long totalWeight = 0;
    int components = 0;
};

/**
 * Kruskal's minimum spanning forest algorithm.
 *
 * The graph stores undirected roads as two directed adjacency entries. The
 * implementation considers only one entry for each pair of node IDs.
 */
MstResult kruskalMst(const core::Graph& graph);

} // namespace algorithms
