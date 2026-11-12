// ===== FILE: src/ui/InfoPanel.h =====
#pragma once
#include <SFML/Graphics.hpp>
#include "../algorithms/PathResult.h"
#include <array>
#include <string>

namespace ui {

enum class WeightType { Distance, Time, Cost };

/**
 * @file InfoPanel.h
 * @brief Side information panel rendering live statistics, algorithm states, and comparison table.
 */
class InfoPanel {
public:
    InfoPanel() = default;
    void init(const sf::Font& font, float panelX, float panelWidth, float windowHeight);

    void setSource(const std::string& name);
    void setDestination(const std::string& name);
    void setResult(const PathResult& result);
    void setComparisonResults(const std::array<PathResult, 3>& results);
    void setActiveAlgorithm(int index); // 0=Dijkstra, 1=A*, 2=BFS
    void setStepMode(bool on);
    void setWeightType(WeightType type);
    void setLiveStats(int nodesExplored, long long distSoFar);

    void draw(sf::RenderWindow& window);

private:
    const sf::Font* m_font = nullptr;
    float m_panelX = 1080.0f;
    float m_panelWidth = 320.0f;
    float m_windowHeight = 800.0f;

    std::string m_sourceName = "-";
    std::string m_destName = "-";
    PathResult m_currentResult;
    std::array<PathResult, 3> m_comparisonResults;
    int m_activeAlgorithm = 0;
    bool m_stepMode = false;
    WeightType m_weightType = WeightType::Distance;
    int m_liveNodesExplored = 0;
    long long m_liveDistSoFar = 0;
    bool m_hasResult = false;
    bool m_hasComparison = false;

    sf::Text makeText(const std::string& str, unsigned int size, sf::Color color, float x, float y) const;
    std::string weightTypeToString(WeightType type) const;
};

} // namespace ui
