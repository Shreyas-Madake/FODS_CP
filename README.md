# 🗺️ RouteX — Interactive Graph-Based Route Planner & Shortest Path Visualizer
*Final Semester Project — Fundamentals of Data Structures (FODS)*

RouteX is a professional desktop application built from scratch in C++17 and SFML. It visualizes core graph data structures and pathfinding algorithms (Dijkstra, A*, BFS) over a realistic Indian city network, complete with live step-by-step animation, a moving vehicle simulation, and a side-by-side comparison panel.

---

## 📸 Screenshots

> *See `docs/SCREENSHOTS.md` for placeholder instructions and expected visuals.*

---

## ✨ Features

- **Hand-Implemented Data Structures**: No STL shortcuts for core structures (Adjacency List Graph, Binary Min-Heap, Hash Map wrapper, Parent-Pointer Array).
- **Three Pathfinding Algorithms**: Dijkstra's Algorithm, A* Search (with Euclidean heuristic), and Breadth-First Search (BFS).
- **Interactive SFML Visualizer**:
  - Click nodes to select Source (Green) and Destination (Crimson).
  - Pan (WASD / Middle Mouse) & Zoom (Scroll Wheel).
  - Hover tooltips and glow effects.
- **Live Algorithm Animation**:
  - Nodes flash Yellow when visiting, transition to Green when finalized.
  - Step-by-step mode (`SPACE`) for Viva demonstrations.
  - Glowing red path animation upon completion.
- **Moving Vehicle Simulation**: A glowing cyan vehicle marker with a fading trail travels along the shortest path.
- **Side Info Panel**: Real-time stats, active algorithm indicator, and side-by-side comparison table.
- **Robust Error Handling**: Handles disconnected graphs, missing files, and malformed CSV entries gracefully.

---

## 📚 Core Data Structures & Why Each Was Chosen

### 1. Graph (Adjacency List)
- **Design**: Implemented as `std::vector<std::vector<Edge>>` alongside a vertex list and a hash map (`std::unordered_map<int, int>`) for O(1) ID-to-index lookup.
- **Why**: Adjacency lists are space-efficient for sparse graphs ($O(V + E)$ space) compared to adjacency matrices ($O(V^2)$), and iterate over outgoing neighbors in $O(degree(v))$ time.

### 2. Binary Min-Heap (`MinHeap<T, Compare>`)
- **Design**: Hand-written binary min-heap supporting `push`, `pop`, `top`, `size`, `empty`, and `decreaseKey`. Fully template-based with custom comparator support.
- **Why**: Essential for greedy shortest-path algorithms. Hand-implementing the sift-up / sift-down array logic demonstrates mastery of heap invariants without wrapping `std::priority_queue`.

### 3. Hash Map Wrapper
- **Design**: Uses `std::unordered_map<int, int>` to map external user node IDs to internal contiguous vector indices.
- **Why**: Provides $O(1)$ average-time lookup, enabling instant node mapping during graph traversal and CSV parsing.

### 4. Parent-Pointer Array
- **Design**: An array/vector storing predecessor indices (`parent[u] = v`).
- **Why**: Lightweight $O(V)$ space structure that allows fast $O(PathLength)$ reconstruction of the final path by backtracking from destination to source.

---

## 📊 Algorithm Complexity Analysis

| Algorithm | Time Complexity | Space Complexity | Optimal? | Key Characteristic |
| :--- | :--- | :--- | :--- | :--- |
| **Dijkstra's Algorithm** | $O((V + E) \log V)$ | $O(V)$ | Yes | Explores uniformly by distance |
| **A\* Search** | $O((V + E) \log V)$ worst | $O(V)$ | Yes (admissible) | Uses Euclidean heuristic to goal |
| **Breadth-First Search** | $O(V + E)$ | $O(V)$ | No (edge count) | Explores level-by-level (unweighted) |

---

## 🗂️ Architecture Overview

```
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
```

---

## 🛠️ Build Instructions

### Prerequisites
- C++17 compliant compiler (GCC 9+, Clang 10+, MSVC 2019+)
- CMake 3.16+
- SFML 2.5+ library

### Windows (MSVC & vcpkg / Manual)
1. Install SFML (e.g., `vcpkg install sfml:x64-windows`).
2. Clone or open the repository.
3. Configure & Build:
   ```powershell
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
   cmake --build . --config Release
   ```

### Linux (Ubuntu / Debian)
1. Install dependencies:
   ```bash
   sudo apt update
   sudo apt install build-essential cmake libsfml-dev
   ```
2. Build:
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```

---

## 🚀 How to Run
1. Place a valid TrueType font (`font.ttf`) into the `assets/` directory (e.g., DejaVuSans.ttf or Arial). If missing, a console warning is logged.
2. Run the compiled executable from the build directory (data files are automatically copied via CMake).

---

## 🎮 Controls Reference

| Key / Action | Description |
| :--- | :--- |
| **Left Click** (on node) | Select Source (first click) / Destination (second click) |
| **Right Click** | Reset selection |
| **Middle Click + Drag** | Pan canvas |
| **Mouse Scroll** | Zoom in / out |
| **[ENTER]** | Run active algorithm |
| **[SPACE]** | Toggle step-by-step mode (or advance one step) |
| **[1] / [2] / [3]** | Switch algorithm (1: Dijkstra, 2: A*, 3: BFS) |
| **[R]** | Full application reset |
| **[C]** | Clear current animation |
| **[W/A/S/D]** | Pan view |
| **[ESC]** | Quit application |

---

## 📝 Author
Final Semester Project — Fundamentals of Data Structures (FODS).
