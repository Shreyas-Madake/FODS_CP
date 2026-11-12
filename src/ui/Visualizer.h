// ===== FILE: src/ui/Visualizer.h =====
#pragma once
#include <SFML/Graphics.hpp>
#include "../core/Graph.h"
#include "../algorithms/PathResult.h"
#include "InfoPanel.h"
#include "Animator.h"
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
    int m_selectionState = 0; // 0=none, 1=source selected, 2=both selected
    int m_activeAlgorithm = 0; // 0=Dijkstra, 1=A*, 2=BFS
    int m_hoveredNode = -1;

    PathResult m_currentResult;
    std::array<PathResult, 3> m_comparisonResults;
    bool m_hasResult = false;
    bool m_hasComparison = false;

    InfoPanel m_infoPanel;
    Animator m_animator;

    // Pan / Zoom
    float m_zoom = 1.0f;
    bool m_isPanning = false;
    sf::Vector2i m_lastMousePos;

    void handleEvents();
    void update(float deltaTime);
    void render();

    void runAlgorithm(int algorithmIndex);
    void runAllAlgorithms();
    void switchWeightType(WeightType newType);
    void resetSelection();
    void resetAll();

    void drawEdges();
    void drawNodes();
    void drawVehicle(const Animator::AnimationState& state);
    void drawHintText();

    int getNodeAtPosition(sf::Vector2f worldPos) const;
    sf::Vector2f screenToWorld(sf::Vector2i screenPos) const;
    void autoFitView();
};

} // namespace ui
