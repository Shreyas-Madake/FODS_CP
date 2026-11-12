// ===== FILE: src/ui/Animator.cpp =====
#include "Animator.h"
#include <cmath>

namespace ui {

Animator::Animator() {
    reset();
}

void Animator::startExploration(const PathResult& result, const core::Graph& graph) {
    m_result = result;
    m_graph = &graph;
    m_explorationIdx = 0;
    m_explorationTimer = 0.0f;
    m_visitedSoFar.clear();
    m_pathEdgeIdx = 0;
    m_pathRevealTimer = 0.0f;
    m_vehicleProgress = 0.0f;
    m_pathPositions.clear();
    m_trail.clear();

    if (!m_result.found || m_result.explorationOrder.empty()) {
        m_phase = Phase::Done;
    } else {
        m_phase = Phase::Exploring;
    }
}

void Animator::setStepMode(bool on) {
    m_stepMode = on;
    m_stepReady = true;
}

bool Animator::isStepMode() const { return m_stepMode; }

void Animator::advanceStep() {
    if (m_phase == Phase::Exploring && m_stepMode) {
        m_stepReady = true;
    }
}

void Animator::update(float deltaTime) {
    if (m_phase == Phase::Idle || m_phase == Phase::Done) return;

    if (m_phase == Phase::Exploring) {
        bool advance = false;
        if (m_stepMode) {
            if (m_stepReady) {
                advance = true;
                m_stepReady = false;
            }
        } else {
            m_explorationTimer += deltaTime;
            if (m_explorationTimer >= m_nodeFlashDuration) {
                m_explorationTimer = 0.0f;
                advance = true;
            }
        }

        if (advance) {
            if (m_explorationIdx < static_cast<int>(m_result.explorationOrder.size())) {
                int nodeId = m_result.explorationOrder[m_explorationIdx];
                m_visitedSoFar.push_back(nodeId);
                m_explorationIdx++;
            }

            if (m_explorationIdx >= static_cast<int>(m_result.explorationOrder.size())) {
                // Transition to path reveal
                if (m_result.found && !m_result.path.empty()) {
                    m_phase = Phase::PathReveal;
                    m_pathEdgeIdx = 0;
                    // Prepare path positions
                    for (int id : m_result.path) {
                        const auto& node = m_graph->getNode(id);
                        m_pathPositions.push_back(sf::Vector2f(static_cast<float>(node.x), static_cast<float>(node.y)));
                    }
                } else {
                    m_phase = Phase::Done;
                }
            }
        }
    } else if (m_phase == Phase::PathReveal) {
        m_pathRevealTimer += deltaTime;
        if (m_pathRevealTimer >= m_edgeRevealDuration) {
            m_pathRevealTimer = 0.0f;
            m_pathEdgeIdx++;

            int totalEdges = std::max(0, static_cast<int>(m_result.path.size()) - 1);
            if (m_pathEdgeIdx >= totalEdges) {
                if (m_pathPositions.size() >= 2) {
                    m_phase = Phase::Vehicle;
                    m_vehicleProgress = 0.0f;
                } else {
                    m_phase = Phase::Done;
                }
            }
        }
    } else if (m_phase == Phase::Vehicle) {
        m_vehicleProgress += deltaTime / m_vehicleDuration;
        if (m_vehicleProgress >= 1.0f) {
            m_vehicleProgress = 1.0f;
            m_phase = Phase::Done;
        }

        // Interpolate vehicle position along path
        if (m_pathPositions.size() >= 2) {
            // Compute total path length
            float totalLen = 0.0f;
            std::vector<float> segmentLens;
            for (size_t i = 0; i < m_pathPositions.size() - 1; ++i) {
                float dx = m_pathPositions[i+1].x - m_pathPositions[i].x;
                float dy = m_pathPositions[i+1].y - m_pathPositions[i].y;
                float len = std::sqrt(dx*dx + dy*dy);
                segmentLens.push_back(len);
                totalLen += len;
            }

            float targetDist = m_vehicleProgress * totalLen;
            float accumulated = 0.0f;
            sf::Vector2f currentPos = m_pathPositions.front();

            for (size_t i = 0; i < segmentLens.size(); ++i) {
                if (accumulated + segmentLens[i] >= targetDist) {
                    float t = (targetDist - accumulated) / segmentLens[i];
                    currentPos.x = m_pathPositions[i].x + t * (m_pathPositions[i+1].x - m_pathPositions[i].x);
                    currentPos.y = m_pathPositions[i].y + t * (m_pathPositions[i+1].y - m_pathPositions[i].y);
                    break;
                }
                accumulated += segmentLens[i];
                currentPos = m_pathPositions[i+1];
            }

            m_trail.insert(m_trail.begin(), currentPos);
            if (m_trail.size() > kTrailLength) {
                m_trail.pop_back();
            }
        }
    }
}

bool Animator::isExplorationDone() const { return m_phase != Phase::Idle && m_phase != Phase::Exploring; }
bool Animator::isPathAnimationDone() const { return m_phase == Phase::Vehicle || m_phase == Phase::Done; }
bool Animator::isVehicleDone() const { return m_phase == Phase::Done; }
bool Animator::isAnimating() const { return m_phase != Phase::Idle && m_phase != Phase::Done; }

Animator::AnimationState Animator::getState() const {
    AnimationState state;
    if (m_phase == Phase::Exploring && !m_visitedSoFar.empty()) {
        state.currentNodeId = m_visitedSoFar.back();
        if (m_visitedSoFar.size() > 1) {
            state.visitedNodes.assign(m_visitedSoFar.begin(), m_visitedSoFar.end() - 1);
        }
    } else if (m_phase >= Phase::PathReveal) {
        state.visitedNodes = m_visitedSoFar;
    }

    if (m_phase >= Phase::PathReveal && !m_result.path.empty()) {
        int totalEdges = static_cast<int>(m_result.path.size()) - 1;
        state.pathEdgesRevealed = std::min(m_pathEdgeIdx, totalEdges);
        for (int i = 0; i < totalEdges; ++i) {
            state.pathEdges.push_back({m_result.path[i], m_result.path[i+1]});
        }
    }

    if (m_phase == Phase::Vehicle || (m_phase == Phase::Done && !m_pathPositions.empty())) {
        state.showVehicle = true;
        if (!m_trail.empty()) {
            state.vehicleX = m_trail.front().x;
            state.vehicleY = m_trail.front().y;
            state.trail = m_trail;
        } else if (!m_pathPositions.empty()) {
            state.vehicleX = m_pathPositions.back().x;
            state.vehicleY = m_pathPositions.back().y;
        }
    }

    return state;
}

int Animator::getNodesExploredSoFar() const {
    return static_cast<int>(m_visitedSoFar.size());
}

void Animator::reset() {
    m_phase = Phase::Idle;
    m_explorationIdx = 0;
    m_explorationTimer = 0.0f;
    m_visitedSoFar.clear();
    m_pathEdgeIdx = 0;
    m_pathRevealTimer = 0.0f;
    m_vehicleProgress = 0.0f;
    m_pathPositions.clear();
    m_trail.clear();
}

} // namespace ui
