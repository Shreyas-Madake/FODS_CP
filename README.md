# 🗺️ RouteX — Interactive Graph-Based Route Planner & Shortest Path Visualizer
**Final Semester Project — Fundamentals of Data Structures (FODS)**

RouteX is a professional desktop application built from scratch in C++17 and SFML. It visualizes core graph data structures and pathfinding algorithms (Dijkstra, A*, BFS) over a realistic Indian city network, complete with live step-by-step animation, a moving vehicle simulation, and a side-by-side comparison panel.

## 📸 Screenshots
*(Add your screenshots here after running the app!)*
![RouteX Interface](docs/screenshot1.png)
![Algorithm Comparison](docs/screenshot2.png)

## ✨ Features
- **Hand-Implemented Data Structures:** No STL shortcuts for core structures (Adjacency List Graph, Binary Min-Heap, Hash Map wrapper, Parent-Pointer Array).
- **Three Pathfinding Algorithms:** Dijkstra's Algorithm, A* Search (with Euclidean heuristic), and Breadth-First Search (BFS).
- **Interactive SFML Visualizer:**
  - Click nodes to select Source (Green) and Destination (Crimson).
  - Pan (WASD / Middle Mouse) & Zoom (Scroll Wheel).
  - Hover tooltips and glow effects.
- **Live Algorithm Animation:**
  - Nodes flash Yellow when visiting, transition to Green when finalized.
  - Step-by-step mode (SPACE) for Viva demonstrations.
  - Glowing red path animation upon completion.
- **Moving Vehicle Simulation:** A glowing cyan vehicle marker with a fading trail travels along the shortest path.
- **Side Info Panel:** Real-time stats, active algorithm indicator, and side-by-side comparison table.
- **Robust Error Handling:** Handles disconnected graphs, missing files, and malformed CSV entries gracefully.

## 📚 Core Data Structures & Why Each Was Chosen
1. **Graph (Adjacency List)**
   - *Design:* Implemented as `std::vector<std::vector<Edge>>` alongside a vertex list and a hash map (`std::unordered_map<int, int>`) for O(1) ID-to-index lookup.
   - *Why:* Adjacency lists are space-efficient for sparse graphs (`O(V + E)` space) compared to adjacency matrices (`O(V^2)`), and iterate over outgoing neighbors in `O(degree(v))` time.
2. **Binary Min-Heap (`MinHeap<T, Compare>`)**
   - *Design:* Hand-written binary min-heap supporting `push`, `pop`, `top`, `size`, `empty`, and `decreaseKey`. Fully template-based with custom comparator support.
   - *Why:* Essential for greedy shortest-path algorithms. Hand-implementing the sift-up / sift-down array logic demonstrates mastery of heap invariants without wrapping `std::priority_queue`.
3. **Hash Map Wrapper**
   - *Design:* Uses `std::unordered_map<int, int>` to map external user node IDs to internal contiguous vector indices.
   - *Why:* Provides `O(1)` average-time lookup, enabling instant node mapping during graph traversal and CSV parsing.
4. **Parent-Pointer Array**
   - *Design:* An array/vector storing predecessor indices (`parent[u] = v`).
   - *Why:* Lightweight `O(V)` space structure that allows fast `O(PathLength)` reconstruction of the final path by backtracking from destination to source.

## 📊 Algorithm Complexity Analysis
| Algorithm | Time Complexity | Space Complexity | Optimal? | Key Characteristic |
| :--- | :--- | :--- | :--- | :--- |
| **Dijkstra's Algorithm** | `O((V + E) log V)` | `O(V)` | Yes | Explores uniformly by distance |
| **A* Search** | `O((V + E) log V)` worst | `O(V)` | Yes (admissible) | Uses Euclidean heuristic to goal |
| **Breadth-First Search** | `O(V + E)` | `O(V)` | No (edge count) | Explores level-by-level (unweighted) |

## 🗂️ Architecture Overview
```text
RouteX/
├── CMakeLists.txt
├── main.cpp
├── assets/
│   └── font.ttf
├── src/
│   ├── core/
│   │   ├── Graph.h / .cpp
│   │   ├── MinHeap.h
│   │   └── CSVParser.h / .cpp
│   ├── algorithms/
│   │   ├── PathResult.h
│   │   ├── Dijkstra.h / .cpp
│   │   ├── AStar.h / .cpp
│   │   └── BFS.h / .cpp
│   ├── ui/
│   │   ├── Visualizer.h / .cpp
│   │   ├── Animator.h / .cpp
│   │   └── InfoPanel.h / .cpp
│   └── utils/
│       ├── Theme.h
│       └── Timer.h
├── data/
│   ├── nodes.csv
│   ├── edges_distance.csv
│   ├── edges_time.csv
│   └── edges_cost.csv
└── docs/
    ├── VIVA_QA.md
    └── SCREENSHOTS.md