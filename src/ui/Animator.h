// ===== FILE: src/ui/Animator.h =====
#pragma once
#include <SFML/Graphics.hpp>
#include "../core/Graph.h"
#include "../algorithms/PathResult.h"
#include <vector>

namespace ui {

/**
 * @file Animator.h
 * @brief Manages live algorithm exploration animation, step-by-step mode, and moving vehicle simulation.
 */
class Animator {
public:
    Animator();

    void startExploration(const PathResult& result, const core::Graph& graph);
    void setStepMode(bool on);
    bool isStepMode() const;
    void advanceStep();

    void update(float deltaTime);

    bool isExplorationDone() const;
    bool isPathAnimationDone() const;
    bool isVehicleDone() const;
    bool isAnimating() const;

    struct AnimationState {
        int currentNodeId = -1;
        std::vector<int> visitedNodes;
        std::vector<std::pair<int, int>> pathEdges;
        int pathEdgesRevealed = 0;
        bool showVehicle = false;
        float vehicleX = 0.0f;
        float vehicleY = 0.0f;
        std::vector<sf::Vector2f> trail;
    };

    AnimationState getState() const;
    int getNodesExploredSoFar() const;
    void reset();

private:
    enum class Phase { Idle, Exploring, PathReveal, Vehicle, Done };
    Phase m_phase = Phase::Idle;

    PathResult m_result;
    const core::Graph* m_graph = nullptr;

    int m_explorationIdx = 0;
    float m_explorationTimer = 0.0f;
    float m_nodeFlashDuration = 0.08f; // 80ms per node
    bool m_stepMode = false;
    bool m_stepReady = false;
    std::vector<int> m_visitedSoFar;

    int m_pathEdgeIdx = 0;
    float m_pathRevealTimer = 0.0f;
    float m_edgeRevealDuration = 0.1f; // 100ms per edge

    float m_vehicleProgress = 0.0f;
    float m_vehicleDuration = 3.0f; // 3 seconds travel time
    std::vector<sf::Vector2f> m_pathPositions;
    std::vector<sf::Vector2f> m_trail;
    static constexpr int kTrailLength = 8;
};

} // namespace ui
