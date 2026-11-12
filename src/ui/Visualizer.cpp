// ===== FILE: src/ui/Visualizer.cpp =====
#include "Visualizer.h"
#include "../utils/Theme.h"
#include "../core/CSVParser.h"
#include "../algorithms/Dijkstra.h"
#include "../algorithms/AStar.h"
#include "../algorithms/BFS.h"
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
            }
            else if (event.key.code == sf::Keyboard::T) {
                // Cycle weight types: Distance -> Time -> Cost -> Distance
                int nextType = (static_cast<int>(m_currentWeightType) + 1) % 3;
                switchWeightType(static_cast<WeightType>(nextType));
            }
            // Pan with WASD
            else if (event.key.code == sf::Keyboard::W) m_graphView.move(0.f, -20.f * m_zoom);
            else if (event.key.code == sf::Keyboard::S) m_graphView.move(0.f, 20.f * m_zoom);
            else if (event.key.code == sf::Keyboard::A) m_graphView.move(-20.f * m_zoom, 0.f);
            else if (event.key.code == sf::Keyboard::D) m_graphView.move(20.f * m_zoom, 0.f);
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Check if click is inside graph area (x < WindowWidth - PanelWidth)
                if (event.mouseButton.x < Theme::WindowWidth - Theme::PanelWidth) {
                    sf::Vector2f worldPos = screenToWorld(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    int clickedNode = getNodeAtPosition(worldPos);

                    if (clickedNode != -1) {
                        if (m_selectionState == 0 || m_selectionState == 2) {
                            m_sourceId = clickedNode;
                            m_destId = -1;
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
                } else {
                    m_hoveredNode = -1;
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
    auto animState = m_animator.getState();
    m_infoPanel.setLiveStats(m_animator.getNodesExploredSoFar(), 0);
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

            if (isPath) {
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
                line[0].color = Theme::kEdgeDefault;
                line[1].color = Theme::kEdgeDefault;
                m_window.draw(line);
            }

            // Draw weight label at midpoint
            if (m_font.getInfo().family != "") {
                sf::Text weightText;
                weightText.setFont(m_font);
                weightText.setString(std::to_string(edge.weight));
                weightText.setCharacterSize(10);
                weightText.setScale(m_zoom, m_zoom);
                weightText.setFillColor(Theme::kTextMuted);
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

        if (id == m_sourceId) {
            fillColor = Theme::kNodeSource;
        } else if (id == m_destId) {
            fillColor = Theme::kNodeDest;
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
    hint.setString("Click a node to set SOURCE");
    hint.setCharacterSize(16);
    hint.setScale(m_zoom, m_zoom);
    hint.setFillColor(Theme::kTextMuted);

    sf::Vector2f center = m_graphView.getCenter();
    sf::Vector2f size = m_graphView.getSize();
    hint.setPosition(center.x - size.x * 0.25f, center.y - size.y * 0.45f);
    m_window.draw(hint);
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
    m_currentWeightType = newType;
    m_infoPanel.setWeightType(newType);

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

        // Re-run if selection active
        if (m_selectionState == 2) {
            runAlgorithm(m_activeAlgorithm);
            runAllAlgorithms();
        }
    }
}

void Visualizer::resetSelection() {
    m_sourceId = -1;
    m_destId = -1;
    m_selectionState = 0;
    m_infoPanel.setSource("-");
    m_infoPanel.setDestination("-");
    m_animator.reset();
    m_hasResult = false;
}

void Visualizer::resetAll() {
    resetSelection();
    autoFitView();
}

} // namespace ui
