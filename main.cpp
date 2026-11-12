// ===== FILE: main.cpp =====
#include "src/core/Graph.h"
#include "src/core/CSVParser.h"
#include "src/ui/Visualizer.h"
#include <iostream>
#include <stdexcept>
#include <string>

/**
 * RouteX - Interactive Graph-Based Route Planner
 * FODS Final Semester Project
 *
 * Entry point for the application. Initializes data structures,
 * loads graph data, and launches the SFML visualizer.
 */

void printBanner() {
    std::cout << "╔══════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║   ____             _       __  __                    ║" << std::endl;
    std::cout << "║  |  _ \\ ___  _   _| |_ _____\\ \\/ /                    ║" << std::endl;
    std::cout << "║  | |_) / _ \\| | | | __/ _ \\\\  /                     ║" << std::endl;
    std::cout << "║  |  _ < (_) | |_| | ||  __//  \\                     ║" << std::endl;
    std::cout << "║  |_| \\_\\___/ \\__,_|\\__\\___/_/\\_\\                    ║" << std::endl;
    std::cout << "║                                                      ║" << std::endl;
    std::cout << "║  Interactive Graph-Based Route Planner               ║" << std::endl;
    std::cout << "║  FODS Final Semester Project                         ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════╝" << std::endl;
}

int main() {
    try {
        printBanner();

        // Data file paths
        std::string nodesPath = "data/nodes.csv";
        std::string edgesDistPath = "data/edges_distance.csv";
        std::string edgesTimePath = "data/edges_time.csv";
        std::string edgesCostPath = "data/edges_cost.csv";
        std::string fontPath = "assets/font.ttf";

        // Initialize Graph and Parser
        core::Graph graph;
        core::CSVParser parser;

        // Load Nodes
        std::cout << "[INFO] Loading nodes from " << nodesPath << "..." << std::endl;
        if (!parser.loadNodes(nodesPath, graph)) {
            std::cerr << "[ERROR] Failed to load nodes. Ensure " << nodesPath << " exists." << std::endl;
            return 1;
        }

        // Load default Edges (Distance)
        std::cout << "[INFO] Loading edges from " << edgesDistPath << "..." << std::endl;
        if (!parser.loadEdges(edgesDistPath, graph)) {
            std::cerr << "[ERROR] Failed to load edges. Ensure " << edgesDistPath << " exists." << std::endl;
            return 1;
        }

        std::cout << "[SUCCESS] Graph loaded with " << graph.getNodeCount() << " nodes and "
                  << graph.getEdgeCount() << " edges." << std::endl;

        // Launch Visualizer
        ui::Visualizer visualizer(graph, fontPath);

        // Register other edge files for weight switching
        visualizer.registerWeightFile(ui::WeightType::Distance, edgesDistPath);
        visualizer.registerWeightFile(ui::WeightType::Time, edgesTimePath);
        visualizer.registerWeightFile(ui::WeightType::Cost, edgesCostPath);

        std::cout << "[INFO] Launching SFML Visualizer..." << std::endl;
        visualizer.run();

        std::cout << "\n[INFO] RouteX closed cleanly. Goodbye!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "[FATAL ERROR] Unknown exception occurred." << std::endl;
        return 1;
    }

    return 0;
}
