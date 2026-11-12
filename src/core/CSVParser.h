// ===== FILE: src/core/CSVParser.h =====
#pragma once
#include "Graph.h"
#include <string>

namespace core {

/**
 * @file CSVParser.h
 * @brief Robust CSV parser for loading nodes and edges into the Graph.
 *
 * Handles file opening errors, comments (#), malformed lines, and trailing spaces.
 */
class CSVParser {
public:
    CSVParser() = default;

    /**
     * @brief Load nodes from CSV file (format: id,x,y,name).
     * @return true on success, false on failure.
     */
    bool loadNodes(const std::string& filepath, Graph& graph);

    /**
     * @brief Load edges from CSV file (format: source_id,target_id,weight).
     * Automatically adds reverse edges to make the graph undirected for bi-directional travel.
     * @return true on success, false on failure.
     */
    bool loadEdges(const std::string& filepath, Graph& graph);
};

} // namespace core
