// ===== FILE: src/core/CSVParser.cpp =====
#include "CSVParser.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace core {

bool CSVParser::loadNodes(const std::string& filepath, Graph& graph) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] CSVParser: Could not open nodes file: " << filepath << std::endl;
        return false;
    }

    std::string line;
    int lineNum = 0;
    int successCount = 0;

    while (std::getline(file, line)) {
        lineNum++;
        // Trim leading/trailing whitespace
        if (line.empty() || line[0] == '#') continue; // Skip comments and empty lines

        std::stringstream ss(line);
        std::string idStr, xStr, yStr, name;

        if (std::getline(ss, idStr, ',') &&
            std::getline(ss, xStr, ',') &&
            std::getline(ss, yStr, ',') &&
            std::getline(ss, name)) {

            try {
                int id = std::stoi(idStr);
                double x = std::stod(xStr);
                double y = std::stod(yStr);

                // Trim name whitespace
                name.erase(0, name.find_first_not_of(" \t\r\n"));
                name.erase(name.find_last_not_of(" \t\r\n") + 1);

                graph.addNode(id, x, y, name);
                successCount++;
            } catch (const std::exception& e) {
                std::cerr << "[WARN] CSVParser: Malformed node line " << lineNum << " in " << filepath
                          << " (" << e.what() << "). Skipping." << std::endl;
            }
        } else {
            std::cerr << "[WARN] CSVParser: Invalid format on line " << lineNum << " in " << filepath << ". Skipping." << std::endl;
        }
    }

    std::cout << "[INFO] CSVParser: Successfully loaded " << successCount << " nodes from " << filepath << std::endl;
    return successCount > 0;
}

bool CSVParser::loadEdges(const std::string& filepath, Graph& graph) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] CSVParser: Could not open edges file: " << filepath << std::endl;
        return false;
    }

    std::string line;
    int lineNum = 0;
    int successCount = 0;

    while (std::getline(file, line)) {
        lineNum++;
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string srcStr, tgtStr, weightStr;

        if (std::getline(ss, srcStr, ',') &&
            std::getline(ss, tgtStr, ',') &&
            std::getline(ss, weightStr)) {

            try {
                int src = std::stoi(srcStr);
                int tgt = std::stoi(tgtStr);
                long long weight = std::stoll(weightStr);

                if (graph.hasNode(src) && graph.hasNode(tgt)) {
                    // Add undirected edges (both directions) for route planning
                    graph.addEdge(src, tgt, weight);
                    graph.addEdge(tgt, src, weight);
                    successCount++;
                } else {
                    std::cerr << "[WARN] CSVParser: Edge line " << lineNum << " in " << filepath
                              << " references non-existent node(s) (" << src << " -> " << tgt << "). Skipping." << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "[WARN] CSVParser: Malformed edge line " << lineNum << " in " << filepath
                          << " (" << e.what() << "). Skipping." << std::endl;
            }
        } else {
            std::cerr << "[WARN] CSVParser: Invalid edge format on line " << lineNum << " in " << filepath << ". Skipping." << std::endl;
        }
    }

    std::cout << "[INFO] CSVParser: Successfully loaded " << successCount << " edge pairs from " << filepath << std::endl;
    return successCount > 0;
}

} // namespace core
