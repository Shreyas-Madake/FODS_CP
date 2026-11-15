// ===== FILE: src/ui/Visualizer.h =====
#pragma once
#include <SFML/Graphics.hpp>
#include "../core/Graph.h"
#include "../algorithms/PathResult.h"
#include "InfoPanel.h"
#include "Animator.h"
#include "../algorithms/Kruskal.h"
#include <array>
#include <string>
#include <unordered_map>

namespace ui {

/**
 * @file Visualizer.h
 * @brief Main rendering and event loop class for RouteX using SFML.
 */
class Visualizer {
public:
    Visualizer(const core::Graph& graph, const std::string& fontPath);
    void run();
    void registerWeightFile(WeightType type, const std::string& filepath);

private:
    sf::RenderWindow m_window;
    sf::Font m_font;
    sf::View m_graphView;

    core::Graph m_graph;
    std::unordered_map<int, std::string> m_weightFiles;
    WeightType m_currentWeightType = WeightType::Distance;

    int m_sourceId = -1;
    int m_destId = -1;
    std::vector<int> m_waypoints;
    bool m_tspActive = false;
    PathResult m_tspResult;
    int m_selectionState = 0; // 0=none, 1=source selected, 2=both selected
    int m_activeAlgorithm = 0; // 0=Dijkstra, 1=A*, 2=BFS
    int m_hoveredNode = -1;

    // Roadblocks
    std::pair<int, int> m_hoveredEdge = {-1, -1};

    PathResult m_currentResult;
    std::array<PathResult, 3> m_comparisonResults;
    bool m_hasResult = false;
    bool m_hasComparison = false;

    bool m_mstMode = false;
    std::vector<std::pair<int, int>> m_mstEdges;
    long long m_mstTotalWeight = 0;
    int m_mstComponents = 0;
    std::size_t m_mstVisibleEdges = 0;
    float m_mstRevealTimer = 0.0f;

    InfoPanel m_infoPanel;
    Animator m_animator;
    float m_pulseTimer = 0.0f; // For glowing pulse animation on source/dest

    // Pan / Zoom
    float m_zoom = 1.0f;
    bool m_isPanning = false;
    sf::Vector2i m_lastMousePos;

    void handleEvents();
    void update(float deltaTime);
    void render();

    void runAlgorithm(int algorithmIndex);
    void runAllAlgorithms();
    void runWaypointRoute(int algorithmIndex);
    void runTsp();
    void switchWeightType(WeightType newType);
    void resetSelection();
    void resetAll();
    void runMst();
    void resetMst();

    void drawEdges();
    void drawNodes();
    void drawVehicle(const Animator::AnimationState& state);
    void drawHintText();

    int getNodeAtPosition(sf::Vector2f worldPos) const;
    std::pair<int, int> getEdgeAtPosition(sf::Vector2f worldPos) const;
    sf::Vector2f screenToWorld(sf::Vector2i screenPos) const;
    void autoFitView();
};

} // namespace ui
