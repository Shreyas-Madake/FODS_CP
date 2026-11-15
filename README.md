# 🗺️ RouteX — Interactive Graph-Based Route Planner & Shortest Path Visualizer
**Final Semester Project — Fundamentals of Data Structures (FODS)**

**RouteX** is a professional, feature-rich desktop application built from scratch in **C++17** and **SFML**. It visualizes core graph data structures and pathfinding algorithms over a realistic city network, featuring live animated exploration, a moving vehicle simulation, real-time roadblocks, multi-stop waypoints, metric switching (Distance, Time, Cost), Kruskal's Minimum Spanning Tree, and Greedy Traveling Salesperson (TSP) tours.

---

## 🌟 Key Features & "Wow" Capabilities

1. **Dynamic Roadblocks (`Ctrl + Click`)**
   - Toggle traffic blockades on any road in real-time. All pathfinding algorithms instantly recalculate and route around the blockage.
2. **Multi-Stop Waypoints (`Shift + Click`)**
   - Plan complex trips (`Source -> WP1 -> WP2 -> Destination`) with automatic segment chaining and summed metrics.
3. **Multi-Metric Weight Switching (`[T]` key)**
   - Instantly toggle datasets to optimize routes by **Distance (km)**, **Travel Time (min)**, or **Toll Cost (Rs)**.
4. **Greedy Traveling Salesperson Tour (`[P]` key)**
   - Compute and render a complete Hamiltonian tour visiting every city on the map starting from the source.
5. **Kruskal's Minimum Spanning Tree (`[M]` key)**
   - Reveal the optimal minimum spanning forest across the graph with animated edge generation.
6. **Hand-Implemented Core Data Structures**
   - Built without STL algorithm wrappers for fundamental structures (Adjacency List, Binary Min-Heap, Union-Find / Disjoint Set).
7. **Live Side-by-Side Algorithm Comparison**
   - Compare Dijkstra, A*, and BFS simultaneously on nodes explored, execution microseconds, and total weight.

---

## 📚 Core Data Structures & Why Each Was Chosen

| Data Structure | Implementation File | Design & Time Complexity | Real-World Analogy & Purpose |
| :--- | :--- | :--- | :--- |
| **Graph (Adjacency List)** | `src/core/Graph.h` | `std::vector<std::vector<Edge>>` + Hash Map ID lookup. $O(1)$ edge/node access, $O(V+E)$ space. | Address book / Switchboard. Stores only existing roads, avoiding matrix memory waste. |
| **Binary Min-Heap** | `src/core/MinHeap.h` | Custom template heap backed by `std::vector`. $O(\log n)$ push/pop, $O(1)$ top. | Hospital Emergency Room Triage. Always processes the closest tentative node first. |
| **Disjoint Set (Union-Find)** | `src/algorithms/Kruskal.cpp` | Path compression & rank optimization. Near $O(1)$ find/unite. | Corporate Mergers. Detects and prevents cycles in Kruskal's MST algorithm. |
| **Parent-Pointer Array** | `src/algorithms/` | `std::unordered_map<int, int> parent`. $O(V)$ space. | Breadcrumb Trail. Backtracks from destination to source to reconstruct paths. |

---

## 📊 Algorithm Complexity Summary

| Algorithm | Time Complexity | Space Complexity | Optimality | Key Characteristic |
| :--- | :--- | :--- | :--- | :--- |
| **Dijkstra's Algorithm** | $O((V + E) \log V)$ | $O(V)$ | Optimal | Explores uniformly by edge weight |
| **A* Search** | $O((V + E) \log V)$ worst | $O(V)$ | Optimal (admissible) | Uses Euclidean heuristic to bias search towards goal |
| **Breadth-First Search** | $O(V + E)$ | $O(V)$ | Unweighted only | Explores level-by-level |
| **Kruskal's MST** | $O(E \log E)$ | $O(V + E)$ | Optimal | Greedy edge sorting with Union-Find cycle check |
| **Greedy TSP Tour** | $O(V^2)$ | $O(V)$ | Heuristic | Nearest-neighbor tour construction |

---

## 🗂️ Project Architecture

```text
RouteX/
├── CMakeLists.txt
├── main.cpp
├── VIVA_CHEATSHEET.md          # Complete viva preparation guide
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
│   │   ├── BFS.h / .cpp
│   │   └── Kruskal.h / .cpp
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
    └── VIVA_QA.md
```

---

## 🚀 Getting Started & Controls

### Build & Run
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
cd Release
.\RouteX.exe
```

### Controls Reference
- **Left Click Node**: Select Source (Green) or Destination (Crimson).
- **`Shift + Click` Node**: Add / remove intermediate Waypoints (Blue).
- **`Ctrl + Click` Edge**: Toggle a Roadblock (Red-Orange) with instant re-routing.
- **`ENTER`**: Run active algorithm and comparison suite.
- **`1`, `2`, `3`**: Switch active algorithm (Dijkstra, A*, BFS).
- **`T`**: Cycle weight metrics (Distance $\rightarrow$ Time $\rightarrow$ Cost).
- **`P`**: Compute & visualize Greedy TSP tour.
- **`M`**: Reveal Kruskal's Minimum Spanning Tree.
- **`SPACE`**: Step-by-step animation mode.
- **`R`**: Reset selection & view.
- **`WASD` / Middle-Mouse Drag / Scroll Wheel**: Pan and Zoom.
- **`ESC`**: Exit application.
