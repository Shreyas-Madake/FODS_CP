// ===== FILE: src/ui/InfoPanel.cpp =====
#include "InfoPanel.h"
#include "../utils/Theme.h"
#include <iomanip>
#include <sstream>

namespace ui {

void InfoPanel::init(const sf::Font& font, float panelX, float panelWidth, float windowHeight) {
    m_font = &font;
    m_panelX = panelX;
    m_panelWidth = panelWidth;
    m_windowHeight = windowHeight;
}

void InfoPanel::setSource(const std::string& name) { m_sourceName = name; }
void InfoPanel::setDestination(const std::string& name) { m_destName = name; }
void InfoPanel::setResult(const PathResult& result) {
    m_currentResult = result;
    m_hasResult = true;
}
void InfoPanel::setComparisonResults(const std::array<PathResult, 3>& results) {
    m_comparisonResults = results;
    m_hasComparison = true;
}
void InfoPanel::setActiveAlgorithm(int index) { m_activeAlgorithm = index; }
void InfoPanel::setStepMode(bool on) { m_stepMode = on; }
void InfoPanel::setWeightType(WeightType type) { m_weightType = type; }
void InfoPanel::setLiveStats(int nodesExplored, long long distSoFar) {
    m_liveNodesExplored = nodesExplored;
    m_liveDistSoFar = distSoFar;
}

std::string InfoPanel::weightTypeToString(WeightType type) const {
    switch (type) {
        case WeightType::Distance: return "Distance (km)";
        case WeightType::Time:     return "Travel Time (min)";
        case WeightType::Cost:     return "Cost (Rs)";
    }
    return "Distance";
}

sf::Text InfoPanel::makeText(const std::string& str, unsigned int size, sf::Color color, float x, float y) const {
    sf::Text text;
    if (m_font) text.setFont(*m_font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}

void InfoPanel::draw(sf::RenderWindow& window) {
    // Panel Background
    sf::RectangleShape panelBg(sf::Vector2f(m_panelWidth, m_windowHeight));
    panelBg.setPosition(m_panelX, 0);
    panelBg.setFillColor(Theme::ColorPanelBg);
    window.draw(panelBg);

    // Vertical separator line
    sf::RectangleShape separator(sf::Vector2f(2.0f, m_windowHeight));
    separator.setPosition(m_panelX, 0);
    separator.setFillColor(Theme::ColorEdgeDefault);
    window.draw(separator);

    float padding = 15.0f;
    float x = m_panelX + padding;
    float y = 15.0f;

    // 1. Title
    window.draw(makeText("ROUTEX", Theme::FontSizeTitle, Theme::ColorTextWhite, x, y));
    y += 28.0f;
    window.draw(makeText("Interactive Route Planner", Theme::FontSizeSmall, Theme::ColorTextMuted, x, y));
    y += 35.0f;

    // 2. Selection Section
    sf::RectangleShape sec1(sf::Vector2f(m_panelWidth - 30.0f, 70.0f));
    sec1.setPosition(x, y);
    sec1.setFillColor(Theme::ColorSectionBg);
    window.draw(sec1);

    float innerY = y + 8.0f;
    window.draw(makeText("SOURCE      : " + m_sourceName, Theme::FontSizeBody, Theme::ColorNodeSource, x + 10, innerY));
    innerY += 22.0f;
    window.draw(makeText("DESTINATION : " + m_destName, Theme::FontSizeBody, Theme::ColorNodeDest, x + 10, innerY));
    y += 85.0f;

    // 3. Results Section
    sf::RectangleShape sec2(sf::Vector2f(m_panelWidth - 30.0f, 105.0f));
    sec2.setPosition(x, y);
    sec2.setFillColor(Theme::ColorSectionBg);
    window.draw(sec2);

    innerY = y + 8.0f;
    std::string distLabel = "DISTANCE";
    if (m_weightType == WeightType::Time) distLabel = "TIME";
    if (m_weightType == WeightType::Cost) distLabel = "COST";

    std::string distVal = m_hasResult ? std::to_string(m_currentResult.totalDistance) + (m_weightType == WeightType::Distance ? " km" : m_weightType == WeightType::Time ? " min" : " Rs") : "-";
    std::string nodesVal = m_hasResult ? std::to_string(m_currentResult.nodesExplored) : std::to_string(m_liveNodesExplored);
    std::string timeVal = m_hasResult ? std::to_string(m_currentResult.executionTimeMs) + " ms" : "-";

    window.draw(makeText("ACTIVE RESULT (" + (m_hasResult ? m_currentResult.algorithmName : "None") + ")", Theme::FontSizeSmall, Theme::ColorTextMuted, x + 10, innerY));
    innerY += 20.0f;
    window.draw(makeText(distLabel + " : " + distVal, Theme::FontSizeBody, Theme::ColorTextWhite, x + 10, innerY));
    innerY += 20.0f;
    window.draw(makeText("EXPLORED   : " + nodesVal, Theme::FontSizeBody, Theme::ColorTextWhite, x + 10, innerY));
    innerY += 20.0f;
    window.draw(makeText("TIME TAKEN : " + timeVal, Theme::FontSizeBody, Theme::ColorTextWhite, x + 10, innerY));
    y += 120.0f;

    // 4. Algorithm Selector
    sf::RectangleShape sec3(sf::Vector2f(m_panelWidth - 30.0f, 95.0f));
    sec3.setPosition(x, y);
    sec3.setFillColor(Theme::ColorSectionBg);
    window.draw(sec3);

    innerY = y + 8.0f;
    window.draw(makeText("SELECT ALGORITHM", Theme::FontSizeSmall, Theme::ColorTextMuted, x + 10, innerY));
    innerY += 20.0f;
    window.draw(makeText("[1] Dijkstra" + std::string(m_activeAlgorithm == 0 ? "  * ACTIVE" : ""), Theme::FontSizeBody, m_activeAlgorithm == 0 ? Theme::ColorActive : Theme::ColorTextWhite, x + 10, innerY));
    innerY += 20.0f;
    window.draw(makeText("[2] A* Search" + std::string(m_activeAlgorithm == 1 ? "  * ACTIVE" : ""), Theme::FontSizeBody, m_activeAlgorithm == 1 ? Theme::ColorActive : Theme::ColorTextWhite, x + 10, innerY));
    innerY += 20.0f;
    window.draw(makeText("[3] BFS (Unweighted)" + std::string(m_activeAlgorithm == 2 ? "  * ACTIVE" : ""), Theme::FontSizeBody, m_activeAlgorithm == 2 ? Theme::ColorActive : Theme::ColorTextWhite, x + 10, innerY));
    y += 110.0f;

    // 5. Comparison Table
    sf::RectangleShape sec4(sf::Vector2f(m_panelWidth - 30.0f, 90.0f));
    sec4.setPosition(x, y);
    sec4.setFillColor(Theme::ColorSectionBg);
    window.draw(sec4);

    innerY = y + 8.0f;
    window.draw(makeText("COMPARISON (Last Run)", Theme::FontSizeSmall, Theme::ColorTextMuted, x + 10, innerY));
    innerY += 20.0f;
    if (m_hasComparison) {
        for (int i = 0; i < 3; ++i) {
            const auto& res = m_comparisonResults[i];
            std::string line = res.algorithmName + " : " + std::to_string(res.nodesExplored) + " nodes, " + std::to_string(res.executionTimeMs).substr(0, 5) + "ms";
            window.draw(makeText(line, Theme::FontSizeSmall, Theme::ColorTextWhite, x + 10, innerY));
            innerY += 18.0f;
        }
    } else {
        window.draw(makeText("Press [ENTER] to run & compare", Theme::FontSizeSmall, Theme::ColorTextMuted, x + 10, innerY));
    }
    y += 105.0f;

    // 6. Weight Type & Controls
    sf::RectangleShape sec5(sf::Vector2f(m_panelWidth - 30.0f, 130.0f));
    sec5.setPosition(x, y);
    sec5.setFillColor(Theme::ColorSectionBg);
    window.draw(sec5);

    innerY = y + 8.0f;
    window.draw(makeText("METRIC : " + weightTypeToString(m_weightType), Theme::FontSizeSmall, Theme::ColorTextMuted, x + 10, innerY));
    innerY += 22.0f;
    window.draw(makeText("[ENTER] Run Algorithm", Theme::FontSizeSmall, Theme::ColorTextWhite, x + 10, innerY));
    innerY += 18.0f;
    window.draw(makeText("[SPACE] Step Mode: " + std::string(m_stepMode ? "ON" : "OFF"), Theme::FontSizeSmall, Theme::ColorTextWhite, x + 10, innerY));
    innerY += 18.0f;
    window.draw(makeText("[R] Reset All   [C] Clear Anim", Theme::FontSizeSmall, Theme::ColorTextWhite, x + 10, innerY));
    innerY += 18.0f;
    window.draw(makeText("[W/A/S/D/Scroll] Pan & Zoom", Theme::FontSizeSmall, Theme::ColorTextWhite, x + 10, innerY));
}

} // namespace ui
