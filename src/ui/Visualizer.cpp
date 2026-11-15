// ===== FILE: src/ui/Visualizer.cpp =====
#include "Visualizer.h"
#include "../utils/Theme.h"
#include "../core/CSVParser.h"
#include "../algorithms/Dijkstra.h"
#include "../algorithms/AStar.h"
#include "../algorithms/BFS.h"
#include "../utils/Timer.h"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace ui {

Visualizer::Visualizer(const core::Graph& graph, const std::string& fontPath)
    : m_graph(graph) {
    if (!m_font.loadFromFile(fontPath)) {
        std::cerr << "[WARN] Visualizer: Could not load font from " << fontPath
                  << ". Text labels may not render properly." << std::endl;
    }

    float winW = Theme::WindowWidth;
    float winH = Theme::WindowHeight;
    float panelW = Theme::PanelWidth;

    m_window.create(sf::VideoMode(static_cast<unsigned int>(winW), static_cast<unsigned int>(winH)),
                    "RouteX - Interactive Graph-Based Route Planner & Shortest Path Visualizer");
    m_window.setFramerateLimit(60);

    // Graph view covers the left area (width = winW - panelW)
    m_graphView.setViewport(sf::FloatRect(0.0f, 0.0f, (winW - panelW) / winW, 1.0f));
    autoFitView();

    m_infoPanel.init(m_font, winW - panelW, panelW, winH);
    m_infoPanel.setWeightType(m_currentWeightType);
}

void Visualizer::registerWeightFile(WeightType type, const std::string& filepath) {
    int key = static_cast<int>(type);
    m_weightFiles[key] = filepath;
}

void Visualizer::autoFitView() {
    auto ids = m_graph.getAllNodeIds();
    if (ids.empty()) return;

    double minX = 1e9, maxX = -1e9, minY = 1e9, maxY = -1e9;
    for (int id : ids) {
        const auto& node = m_graph.getNode(id);
        minX = std::min(minX, node.x);
        maxX = std::max(maxX, node.x);
        minY = std::min(minY, node.y);
        maxY = std::max(maxY, node.y);
    }

    float graphAreaW = Theme::WindowWidth - Theme::PanelWidth;
    float graphAreaH = Theme::WindowHeight;
    const float graphAspect = graphAreaW / graphAreaH;

    double cx = (minX + maxX) / 2.0;
    double cy = (minY + maxY) / 2.0;
    double sizeX = std::max(maxX - minX, 1e-6) * 1.20;
    double sizeY = std::max(maxY - minY, 1e-6) * 1.20;

    // Preserve the graph area's aspect ratio while fitting the complete map.
    if (sizeX / sizeY < graphAspect) {
        sizeX = sizeY * graphAspect;
    } else {
        sizeY = sizeX / graphAspect;
    }

    m_graphView.setCenter(static_cast<float>(cx), static_cast<float>(cy));
    m_graphView.setSize(static_cast<float>(sizeX), static_cast<float>(sizeY));

    // World-units per screen pixel; used to keep UI primitives screen-sized.
    m_zoom = static_cast<float>(sizeX / graphAreaW);
}

void Visualizer::run() {
    sf::Clock deltaClock;
    while (m_window.isOpen()) {
        sf::Time dt = deltaClock.restart();
        handleEvents();
        update(dt.asSeconds());
        render();
    }
}

void Visualizer::handleEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                m_window.close();
            }
            else if (event.key.code == sf::Keyboard::Enter) {
                if (m_selectionState == 2) {
                    runAlgorithm(m_activeAlgorithm);
                    runAllAlgorithms();
                }
            }
            else if (event.key.code == sf::Keyboard::Space) {
                bool currentStep = m_animator.isStepMode();
                if (m_animator.isAnimating()) {
                    m_animator.advanceStep();
                } else {
                    m_animator.setStepMode(!currentStep);
                    m_infoPanel.setStepMode(!currentStep);
                }
            }
            else if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
                m_activeAlgorithm = 0;
                m_infoPanel.setActiveAlgorithm(0);
                if (m_selectionState == 2) runAlgorithm(0);
            }
            else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) {
                m_activeAlgorithm = 1;
                m_infoPanel.setActiveAlgorithm(1);
                if (m_selectionState == 2) runAlgorithm(1);
            }
            else if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3) {
                m_activeAlgorithm = 2;
                m_infoPanel.setActiveAlgorithm(2);
                if (m_selectionState == 2) runAlgorithm(2);
            }
            else if (event.key.code == sf::Keyboard::R) {
                resetAll();
            }
            else if (event.key.code == sf::Keyboard::C) {
                m_animator.reset();
                m_hasResult = false;
                resetMst();
            }
            else if (event.key.code == sf::Keyboard::T) {
                // Cycle weight types: Distance -> Time -> Cost -> Distance
                int nextType = (static_cast<int>(m_currentWeightType) + 1) % 3;
                switchWeightType(static_cast<WeightType>(nextType));
            }
            else if (event.key.code == sf::Keyboard::M) {
                runMst();
            }
            else if (event.key.code == sf::Keyboard::P) {
                runTsp();
            }
            // Pan with WASD
            else if (event.key.code == sf::Keyboard::W) m_graphView.move(0.f, -20.f * m_zoom);
            else if (event.key.code == sf::Keyboard::S) m_graphView.move(0.f, 20.f * m_zoom);
            else if (event.key.code == sf::Keyboard::A) m_graphView.move(-20.f * m_zoom, 0.f);
            else if (event.key.code == sf::Keyboard::D) m_graphView.move(20.f * m_zoom, 0.f);
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Check if click is inside graph area
                if (event.mouseButton.x < Theme::WindowWidth - Theme::PanelWidth) {
                    sf::Vector2f worldPos = screenToWorld(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

                    // Check for Roadblock toggle (Ctrl + Left Click on Edge)
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
                        auto clickedEdge = getEdgeAtPosition(worldPos);
                        if (clickedEdge.first != -1) {
                            m_graph.toggleEdgeBlock(clickedEdge.first, clickedEdge.second);
                            std::cout << "[INFO] Toggled roadblock on edge " << clickedEdge.first << " <-> " << clickedEdge.second << std::endl;
                            // Re-run path if active
                            if (m_selectionState == 2) {
                                runAlgorithm(m_activeAlgorithm);
                                runAllAlgorithms();
                            }
                        }
                    } else {
                        // Standard node selection or Shift+Click Waypoint
                        int clickedNode = getNodeAtPosition(worldPos);
                        if (clickedNode != -1) {
                            bool isShift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
                            if (isShift) {
                                if (clickedNode != m_sourceId && clickedNode != m_destId) {
                                    // Toggle waypoint
                                    auto it = std::find(m_waypoints.begin(), m_waypoints.end(), clickedNode);
                                    if (it != m_waypoints.end()) {
                                        m_waypoints.erase(it);
                                    } else {
                                        m_waypoints.push_back(clickedNode);
                                    }
                                    std::vector<std::string> wpNames;
                                    for (int wId : m_waypoints) wpNames.push_back(m_graph.getNode(wId).name);
                                    m_infoPanel.setWaypoints(wpNames);

                                    if (m_selectionState == 2) {
                                        runAlgorithm(m_activeAlgorithm);
                                    }
                                }
                            } else {
                                if (m_selectionState == 0 || m_selectionState == 2) {
                                    m_sourceId = clickedNode;
                                    m_destId = -1;
                                    m_waypoints.clear();
                                    m_infoPanel.setWaypoints({});
                                    m_selectionState = 1;
                                    m_infoPanel.setSource(m_graph.getNode(m_sourceId).name);
                                    m_infoPanel.setDestination("-");
                                    m_animator.reset();
                                    m_hasResult = false;
                                } else if (m_selectionState == 1) {
                                    if (clickedNode != m_sourceId) {
                                        m_destId = clickedNode;
                                        m_selectionState = 2;
                                        m_infoPanel.setDestination(m_graph.getNode(m_destId).name);
                                        runAlgorithm(m_activeAlgorithm);
                                        runAllAlgorithms();
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (event.mouseButton.button == sf::Mouse::Right) {
                resetSelection();
            }
            else if (event.mouseButton.button == sf::Mouse::Middle) {
                m_isPanning = true;
                m_lastMousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Middle) {
                m_isPanning = false;
            }
        }
        else if (event.type == sf::Event::MouseMoved) {
            if (m_isPanning) {
                sf::Vector2i currentPos(event.mouseMove.x, event.mouseMove.y);
                sf::Vector2i delta = currentPos - m_lastMousePos;
                m_lastMousePos = currentPos;
                m_graphView.move(-static_cast<float>(delta.x) * m_zoom * 0.5f, -static_cast<float>(delta.y) * m_zoom * 0.5f);
            } else {
                // Hover check
                if (event.mouseMove.x < Theme::WindowWidth - Theme::PanelWidth) {
                    sf::Vector2f worldPos = screenToWorld(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                    m_hoveredNode = getNodeAtPosition(worldPos);
                    m_hoveredEdge = getEdgeAtPosition(worldPos);
                } else {
                    m_hoveredNode = -1;
                    m_hoveredEdge = {-1, -1};
                }
            }
        }
        else if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.x < Theme::WindowWidth - Theme::PanelWidth) {
                if (event.mouseWheelScroll.delta > 0) {
                    m_zoom *= 0.9f;
                    m_graphView.zoom(0.9f);
                } else {
                    m_zoom *= 1.1f;
                    m_graphView.zoom(1.1f);
                }
            }
        }
    }
}

void Visualizer::update(float deltaTime) {
    m_animator.update(deltaTime);
    m_infoPanel.setLiveStats(m_animator.getNodesExploredSoFar(), 0);

    if (m_mstMode && m_mstVisibleEdges < m_mstEdges.size()) {
        m_mstRevealTimer += deltaTime;
        constexpr float revealInterval = 0.35f;
        while (m_mstRevealTimer >= revealInterval && m_mstVisibleEdges < m_mstEdges.size()) {
            m_mstRevealTimer -= revealInterval;
            ++m_mstVisibleEdges;
        }
    }
}

void Visualizer::render() {
    m_window.clear(Theme::kWindowBg);

    // 1. Render Graph View
    m_window.setView(m_graphView);
    drawEdges();
    drawNodes();

    auto animState = m_animator.getState();
    if (animState.showVehicle) {
        drawVehicle(animState);
    }

    if (m_selectionState == 0) {
        drawHintText();
    }

    // 2. Render UI Panel (Default View)
    m_window.setView(m_window.getDefaultView());
    m_infoPanel.draw(m_window);

    m_window.display();
}

void Visualizer::drawEdges() {
    if (m_mstMode) {
        for (std::size_t i = 0; i < m_mstVisibleEdges; ++i) {
            const auto [uId, vId] = m_mstEdges[i];
            const auto& uNode = m_graph.getNode(uId);
            const auto& vNode = m_graph.getNode(vId);

            sf::VertexArray glowLine(sf::Lines, 2);
            glowLine[0].position = sf::Vector2f(static_cast<float>(uNode.x), static_cast<float>(uNode.y));
            glowLine[1].position = sf::Vector2f(static_cast<float>(vNode.x), static_cast<float>(vNode.y));
            glowLine[0].color = Theme::kMstEdgeGlow;
            glowLine[1].color = Theme::kMstEdgeGlow;
            m_window.draw(glowLine);

            sf::VertexArray line(sf::Lines, 2);
            line[0] = glowLine[0];
            line[1] = glowLine[1];
            line[0].color = Theme::kMstEdge;
            line[1].color = Theme::kMstEdge;
            m_window.draw(line);
        }
        return;
    }

    auto animState = m_animator.getState();
    auto allIds = m_graph.getAllNodeIds();

    // Map for fast path edge lookup
    std::unordered_map<std::string, bool> pathEdgeMap;
    for (const auto& edgePair : animState.pathEdges) {
        pathEdgeMap[std::to_string(edgePair.first) + "," + std::to_string(edgePair.second)] = true;
        pathEdgeMap[std::to_string(edgePair.second) + "," + std::to_string(edgePair.first)] = true;
    }

    for (int uId : allIds) {
        const auto& uNode = m_graph.getNode(uId);
        for (const auto& edge : m_graph.getNeighbors(uId)) {
            int vId = edge.target;
            if (uId > vId) continue; // Draw undirected edges once

            const auto& vNode = m_graph.getNode(vId);

            sf::VertexArray line(sf::Lines, 2);
            line[0].position = sf::Vector2f(static_cast<float>(uNode.x), static_cast<float>(uNode.y));
            line[1].position = sf::Vector2f(static_cast<float>(vNode.x), static_cast<float>(vNode.y));

            std::string edgeKey = std::to_string(uId) + "," + std::to_string(vId);
            bool isPath = pathEdgeMap[edgeKey];
            bool isBlocked = edge.blocked;
            bool isHoveredEdge = (m_hoveredEdge.first == uId && m_hoveredEdge.second == vId) ||
                                 (m_hoveredEdge.first == vId && m_hoveredEdge.second == uId);

            if (isBlocked) {
                sf::VertexArray blockLine(sf::Lines, 2);
                blockLine[0] = line[0]; blockLine[1] = line[1];
                blockLine[0].color = sf::Color(255, 69, 0, 220);
                blockLine[1].color = sf::Color(255, 69, 0, 220);
                m_window.draw(blockLine);
            } else if (isPath) {
                // Draw glowing thick red line
                sf::VertexArray glowLine(sf::Lines, 2);
                glowLine[0] = line[0]; glowLine[1] = line[1];
                glowLine[0].color = Theme::kEdgePathGlow;
                glowLine[1].color = Theme::kEdgePathGlow;
                m_window.draw(glowLine);

                line[0].color = Theme::kEdgePath;
                line[1].color = Theme::kEdgePath;
                m_window.draw(line);
            } else {
                line[0].color = isHoveredEdge ? sf::Color(255, 255, 255, 200) : Theme::kEdgeDefault;
                line[1].color = isHoveredEdge ? sf::Color(255, 255, 255, 200) : Theme::kEdgeDefault;
                m_window.draw(line);
            }

            // Draw weight label at midpoint
            if (m_font.getInfo().family != "") {
                sf::Text weightText;
                weightText.setFont(m_font);
                weightText.setString(std::to_string(edge.weight) + (edge.blocked ? " [BLOCKED]" : ""));
                weightText.setCharacterSize(10);
                weightText.setScale(m_zoom, m_zoom);
                weightText.setFillColor(edge.blocked ? sf::Color(255, 69, 0) : Theme::kTextMuted);
                weightText.setPosition(static_cast<float>((uNode.x + vNode.x) / 2.0),
                                       static_cast<float>((uNode.y + vNode.y) / 2.0));
                m_window.draw(weightText);
            }
        }
    }
}

void Visualizer::drawNodes() {
    auto animState = m_animator.getState();
    auto allIds = m_graph.getAllNodeIds();

    // Convert visited nodes to lookup map
    std::unordered_map<int, bool> visitedMap;
    for (int id : animState.visitedNodes) visitedMap[id] = true;

    for (int id : allIds) {
        const auto& node = m_graph.getNode(id);
        const float nodeRadius = Theme::kNodeRadiusDefault * m_zoom;
        const float hoverRadius = Theme::kNodeRadiusHover * m_zoom;
        sf::CircleShape circle(nodeRadius);
        circle.setOrigin(nodeRadius, nodeRadius);
        circle.setPosition(static_cast<float>(node.x), static_cast<float>(node.y));

        sf::Color fillColor = Theme::kNodeDefault;

        bool isWaypoint = std::find(m_waypoints.begin(), m_waypoints.end(), id) != m_waypoints.end();

        if (id == m_sourceId) {
            fillColor = Theme::kNodeSource;
        } else if (id == m_destId) {
            fillColor = Theme::kNodeDest;
        } else if (isWaypoint) {
            fillColor = Theme::kNodeWaypoint;
        } else if (id == animState.currentNodeId) {
            fillColor = Theme::kNodeVisiting;
        } else if (visitedMap[id]) {
            fillColor = Theme::kNodeVisited;
        } else if (m_selectionState > 0) {
            fillColor = Theme::kNodeUnvisited;
        }

        circle.setFillColor(fillColor);

        // Hover effect
        if (id == m_hoveredNode) {
            sf::CircleShape glow(hoverRadius);
            glow.setOrigin(hoverRadius, hoverRadius);
            glow.setPosition(static_cast<float>(node.x), static_cast<float>(node.y));
            glow.setFillColor(sf::Color(255, 255, 255, 50));
            m_window.draw(glow);

            circle.setRadius(hoverRadius);
            circle.setOrigin(hoverRadius, hoverRadius);
        }

        m_window.draw(circle);

        // Node Name Label
        if (m_font.getInfo().family != "") {
            sf::Text nameText;
            nameText.setFont(m_font);
            nameText.setString(node.name);
            nameText.setCharacterSize(11);
            nameText.setFillColor(Theme::kTextWhite);

            sf::FloatRect bounds = nameText.getLocalBounds();
            nameText.setOrigin(bounds.width / 2.0f, 0.0f);
            nameText.setScale(m_zoom, m_zoom);
            nameText.setPosition(static_cast<float>(node.x), static_cast<float>(node.y) + nodeRadius + 4.0f * m_zoom);
            m_window.draw(nameText);
        }
    }
}

void Visualizer::drawVehicle(const Animator::AnimationState& state) {
    // Draw trail
    for (size_t i = 0; i < state.trail.size(); ++i) {
        float alpha = static_cast<float>(80 - i * 10);
        if (alpha < 0) alpha = 0;
        const float trailRadius = 6.0f * m_zoom;
        sf::CircleShape trailDot(trailRadius);
        trailDot.setOrigin(trailRadius, trailRadius);
        trailDot.setPosition(state.trail[i]);
        sf::Color trailColor = Theme::kVehicle;
        trailColor.a = static_cast<sf::Uint8>(alpha);
        trailDot.setFillColor(trailColor);
        m_window.draw(trailDot);
    }

    // Draw Vehicle Glow
    const float vehicleRadius = Theme::kVehicleRadius * m_zoom;
    sf::CircleShape glow(vehicleRadius * 1.6f);
    glow.setOrigin(vehicleRadius * 1.6f, vehicleRadius * 1.6f);
    glow.setPosition(state.vehicleX, state.vehicleY);
    glow.setFillColor(Theme::kVehicleGlow);
    m_window.draw(glow);

    // Draw Main Vehicle Dot
    sf::CircleShape vehicle(vehicleRadius);
    vehicle.setOrigin(vehicleRadius, vehicleRadius);
    vehicle.setPosition(state.vehicleX, state.vehicleY);
    vehicle.setFillColor(Theme::kVehicle);
    m_window.draw(vehicle);
}

void Visualizer::drawHintText() {
    if (m_font.getInfo().family == "") return;

    sf::Text hint;
    hint.setFont(m_font);
    if (m_mstMode) {
        hint.setString("Kruskal MST: " + std::to_string(m_mstEdges.size()) +
                       " edges, total " + std::to_string(m_mstTotalWeight) +
                       (m_mstComponents > 1 ? " (forest)" : ""));
    } else {
        hint.setString("Click a node to set SOURCE");
    }
    hint.setCharacterSize(16);
    hint.setScale(m_zoom, m_zoom);
    hint.setFillColor(Theme::kTextMuted);

    sf::Vector2f center = m_graphView.getCenter();
    sf::Vector2f size = m_graphView.getSize();
    hint.setPosition(center.x - size.x * 0.25f, center.y - size.y * 0.45f);
    m_window.draw(hint);
}

std::pair<int, int> Visualizer::getEdgeAtPosition(sf::Vector2f worldPos) const {
    auto allIds = m_graph.getAllNodeIds();
    for (int uId : allIds) {
        const auto& uNode = m_graph.getNode(uId);
        for (const auto& edge : m_graph.getNeighbors(uId)) {
            int vId = edge.target;
            if (uId > vId) continue;
            const auto& vNode = m_graph.getNode(vId);

            // Distance from point to line segment (uNode -> vNode)
            double x1 = uNode.x, y1 = uNode.y;
            double x2 = vNode.x, y2 = vNode.y;
            double px = worldPos.x, py = worldPos.y;

            double l2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
            if (l2 == 0.0) continue;

            double t = ((px - x1) * (x2 - x1) + (py - y1) * (y2 - y1)) / l2;
            t = std::max(0.0, std::min(1.0, t));

            double projX = x1 + t * (x2 - x1);
            double projY = y1 + t * (y2 - y1);

            double dist = std::sqrt((px - projX) * (px - projX) + (py - projY) * (py - projY));
            if (dist <= 6.0f * m_zoom) {
                return {uId, vId};
            }
        }
    }
    return {-1, -1};
}

int Visualizer::getNodeAtPosition(sf::Vector2f worldPos) const {
    auto ids = m_graph.getAllNodeIds();
    for (int id : ids) {
        const auto& node = m_graph.getNode(id);
        double dx = node.x - worldPos.x;
        double dy = node.y - worldPos.y;
        double dist = std::sqrt(dx * dx + dy * dy);
        if (dist <= Theme::kNodeRadiusHover * m_zoom * 0.8f) {
            return id;
        }
    }
    return -1;
}

sf::Vector2f Visualizer::screenToWorld(sf::Vector2i screenPos) const {
    return m_window.mapPixelToCoords(screenPos, m_graphView);
}

void Visualizer::runAlgorithm(int algorithmIndex) {
    if (m_selectionState != 2) return;
    if (!m_waypoints.empty()) {
        runWaypointRoute(algorithmIndex);
        return;
    }

    PathResult res;
    if (algorithmIndex == 0) {
        res = Dijkstra::findPath(m_graph, m_sourceId, m_destId);
    } else if (algorithmIndex == 1) {
        res = AStar::findPath(m_graph, m_sourceId, m_destId);
    } else if (algorithmIndex == 2) {
        res = BFS::findPath(m_graph, m_sourceId, m_destId);
    }

    m_currentResult = res;
    m_hasResult = true;
    m_infoPanel.setResult(res);
    m_animator.startExploration(res, m_graph);
}

void Visualizer::runWaypointRoute(int algorithmIndex) {
    std::vector<int> stops;
    stops.push_back(m_sourceId);
    for (int wp : m_waypoints) stops.push_back(wp);
    stops.push_back(m_destId);

    PathResult combined;
    combined.algorithmName = (algorithmIndex == 0 ? "Dijkstra" : algorithmIndex == 1 ? "A*" : "BFS") + std::string(" (Waypoints)");
    combined.found = true;
    combined.totalDistance = 0;
    combined.nodesExplored = 0;
    long long totalTimeMs = 0;

    std::vector<int> fullPath;
    std::vector<int> fullExploration;

    for (size_t i = 0; i < stops.size() - 1; ++i) {
        int u = stops[i];
        int v = stops[i + 1];
        PathResult subRes;
        if (algorithmIndex == 0) subRes = Dijkstra::findPath(m_graph, u, v);
        else if (algorithmIndex == 1) subRes = AStar::findPath(m_graph, u, v);
        else subRes = BFS::findPath(m_graph, u, v);

        if (!subRes.found) {
            combined.found = false;
            break;
        }

        combined.totalDistance += subRes.totalDistance;
        combined.nodesExplored += subRes.nodesExplored;
        totalTimeMs += subRes.executionTimeMs;

        // Append path (avoid duplicating connection nodes)
        if (i == 0) {
            for (int nId : subRes.path) fullPath.push_back(nId);
        } else {
            for (size_t k = 1; k < subRes.path.size(); ++k) {
                fullPath.push_back(subRes.path[k]);
            }
        }

        for (int nId : subRes.explorationOrder) {
            fullExploration.push_back(nId);
        }
    }

    combined.path = fullPath;
    combined.explorationOrder = fullExploration;
    combined.executionTimeMs = totalTimeMs;

    m_currentResult = combined;
    m_hasResult = true;
    m_infoPanel.setResult(combined);
    m_animator.startExploration(combined, m_graph);
}

void Visualizer::runTsp() {
    if (m_sourceId == -1) return;

    Timer timer;
    auto allIds = m_graph.getAllNodeIds();
    std::unordered_map<int, bool> visited;
    for (int id : allIds) visited[id] = false;

    std::vector<int> tour;
    int curr = m_sourceId;
    visited[curr] = true;
    tour.push_back(curr);

    long long totalTourDist = 0;
    bool complete = true;

    for (size_t step = 1; step < allIds.size(); ++step) {
        int nextNode = -1;
        long long minDist = LLONG_MAX;

        for (const auto& edge : m_graph.getNeighbors(curr)) {
            int neighbor = edge.target;
            if (!visited[neighbor] && edge.weight < minDist) {
                // Verify we can reach it via Dijkstra/direct
                auto pRes = Dijkstra::findPath(m_graph, curr, neighbor);
                if (pRes.found && pRes.totalDistance < minDist) {
                    minDist = pRes.totalDistance;
                    nextNode = neighbor;
                }
            }
        }

        if (nextNode == -1) {
            // Fallback: pick any unvisited node with Dijkstra shortest path
            for (int id : allIds) {
                if (!visited[id]) {
                    auto pRes = Dijkstra::findPath(m_graph, curr, id);
                    if (pRes.found && pRes.totalDistance < minDist) {
                        minDist = pRes.totalDistance;
                        nextNode = id;
                    }
                }
            }
        }

        if (nextNode != -1) {
            auto pRes = Dijkstra::findPath(m_graph, curr, nextNode);
            totalTourDist += pRes.totalDistance;
            visited[nextNode] = true;
            curr = nextNode;
            tour.push_back(curr);
        } else {
            complete = false;
            break;
        }
    }

    // Return to start
    if (complete) {
        auto returnRes = Dijkstra::findPath(m_graph, curr, m_sourceId);
        totalTourDist += returnRes.totalDistance;
        tour.push_back(m_sourceId);
    }

    PathResult tspRes;
    tspRes.algorithmName = "TSP (Greedy Heuristic)";
    tspRes.found = complete;
    tspRes.totalDistance = totalTourDist;
    tspRes.executionTimeMs = timer.elapsedMs();

    // Expand tour into full edge path
    std::vector<int> fullPath;
    for (size_t i = 0; i < tour.size() - 1; ++i) {
        auto subRes = Dijkstra::findPath(m_graph, tour[i], tour[i + 1]);
        if (i == 0) {
            for (int n : subRes.path) fullPath.push_back(n);
        } else {
            for (size_t k = 1; k < subRes.path.size(); ++k) {
                fullPath.push_back(subRes.path[k]);
            }
        }
    }
    tspRes.path = fullPath;
    tspRes.nodesExplored = static_cast<int>(allIds.size());

    m_currentResult = tspRes;
    m_hasResult = true;
    m_tspActive = true;
    m_tspResult = tspRes;
    m_infoPanel.setResult(tspRes);
    m_animator.startExploration(tspRes, m_graph);

    std::cout << "[INFO] Greedy TSP completed: distance = " << totalTourDist << std::endl;
}

void Visualizer::runAllAlgorithms() {
    if (m_selectionState != 2) return;

    PathResult dRes = Dijkstra::findPath(m_graph, m_sourceId, m_destId);
    PathResult aRes = AStar::findPath(m_graph, m_sourceId, m_destId);
    PathResult bRes = BFS::findPath(m_graph, m_sourceId, m_destId);

    m_comparisonResults = {dRes, aRes, bRes};
    m_hasComparison = true;
    m_infoPanel.setComparisonResults(m_comparisonResults);
}

void Visualizer::switchWeightType(WeightType newType) {
    const bool rerunRoute = m_selectionState == 2;
    const bool rerunMst = m_mstMode;

    m_currentWeightType = newType;
    m_infoPanel.setWeightType(newType);

    // Never show a result calculated with a different metric under the new
    // metric's label. Clear it immediately, then optionally calculate again.
    m_currentResult = PathResult{};
    m_comparisonResults = {};
    m_hasResult = false;
    m_hasComparison = false;
    m_animator.reset();
    m_infoPanel.clearResults();

    int key = static_cast<int>(newType);
    if (m_weightFiles.find(key) != m_weightFiles.end()) {
        std::string filepath = m_weightFiles[key];
        std::cout << "[INFO] Switching weight type to " << filepath << std::endl;

        // Reload edges with new weights
        // Clear existing graph edges by rebuilding nodes and reloading
        auto nodes = m_graph.getAllNodeIds();
        // Since Graph doesn't have clearEdges, we re-parse nodes and new edges
        core::Graph newGraph;
        core::CSVParser parser;
        parser.loadNodes("data/nodes.csv", newGraph);
        parser.loadEdges(filepath, newGraph);
        m_graph = newGraph;

        // Re-run the active visualization using the new metric.
        if (rerunMst) {
            runMst();
        } else if (rerunRoute) {
            runAlgorithm(m_activeAlgorithm);
            runAllAlgorithms();
        }
    }
}

void Visualizer::runMst() {
    const algorithms::MstResult result = algorithms::kruskalMst(m_graph);

    m_mstMode = true;
    m_mstEdges = result.edges;
    m_mstTotalWeight = result.totalWeight;
    m_mstComponents = result.components;
    m_mstVisibleEdges = 0;
    m_mstRevealTimer = 0.0f;

    m_sourceId = -1;
    m_destId = -1;
    m_selectionState = 0;
    m_animator.reset();
    m_hasResult = false;
    m_hasComparison = false;
}

void Visualizer::resetMst() {
    m_mstMode = false;
    m_mstEdges.clear();
    m_mstTotalWeight = 0;
    m_mstComponents = 0;
    m_mstVisibleEdges = 0;
    m_mstRevealTimer = 0.0f;
}

void Visualizer::resetSelection() {
    m_sourceId = -1;
    m_destId = -1;
    m_selectionState = 0;
    m_infoPanel.setSource("-");
    m_infoPanel.setDestination("-");
    m_animator.reset();
    m_hasResult = false;
    resetMst();
}

void Visualizer::resetAll() {
    resetSelection();
    autoFitView();
}

} // namespace ui
