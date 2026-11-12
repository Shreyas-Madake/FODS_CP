# 🎯 RouteX — VIVA Q&A Prep Guide

This document contains 25+ comprehensive technical questions and answers designed to help you ace your final viva examination for the RouteX Fundamentals of Data Structures (FODS) project.

---

### 1. Why did you use an Adjacency List instead of an Adjacency Matrix?
**Answer:** An adjacency list stores a vector of edges for each vertex (`std::vector<std::vector<Edge>>`). For sparse road networks (where $E \ll V^2$), an adjacency list requires $O(V + E)$ space, whereas an adjacency matrix requires $O(V^2)$ space. Furthermore, iterating over a node's neighbors takes $O(\text{degree}(v))$ time in an adjacency list, compared to $O(V)$ time in a matrix.

### 2. Why did you hand-implement a Binary Min-Heap instead of using `std::priority_queue`?
**Answer:** The grading rubric explicitly requires hand-implementing core data structures. Hand-implementing the Min-Heap using a vector with `siftUp` and `siftDown` demonstrates a deep understanding of heap invariants, index arithmetic, and logarithmic time bounds without relying on STL wrappers.

### 3. What are the time and space complexities of your Min-Heap operations?
**Answer:**
- `push`: $O(\log n)$ (sift-up)
- `pop`: $O(\log n)$ (sift-down)
- `top`: $O(1)$
- `decreaseKey`: $O(\log n)$
- **Space Complexity:** $O(n)$ to store elements in the underlying vector.

### 4. How does Dijkstra's Algorithm work step by step?
**Answer:**
1. Initialize distance to all nodes as $\infty$, source distance to 0.
2. Push source node into the MinHeap with distance 0.
3. While heap is not empty:
   - Extract node $u$ with minimum distance.
   - If $u$ is already visited/finalized, skip.
   - Mark $u$ as visited.
   - For each neighbor $v$ of $u$ with edge weight $w$:
     - If $dist[u] + w < dist[v]$, update $dist[v] = dist[u] + w$, set `parent[v] = u`, and push into heap.

### 5. Why doesn't Dijkstra's Algorithm work with negative edge weights?
**Answer:** Dijkstra's algorithm relies on the greedy property that once a node is popped from the min-heap, its shortest distance is finalized and can never be improved. Negative edge weights can introduce a path with a smaller total weight through a node that was already finalized, violating this greedy assumption. (Bellman-Ford or SPFA should be used for negative weights).

### 6. What heuristic does A* use, and why must it be admissible?
**Answer:** A* uses the **Euclidean distance** between the current node's $(x, y)$ coordinates and the destination node's coordinates. The heuristic **must be admissible**, meaning it never overestimates the actual cost to reach the goal ($h(n) \le c(n, goal)$). Admissibility guarantees that A* will always find the optimal shortest path.

### 7. Explain the evaluation function $f(n) = g(n) + h(n)$ in A*.
**Answer:**
- $g(n)$: The exact cost (distance/time/cost) from the start node to the current node $n$.
- $h(n)$: The estimated heuristic cost from node $n$ to the destination.
- $f(n)$: The estimated total cost of the cheapest path through $n$. A* always explores the node with the lowest $f(n)$ next.

### 8. How is the path reconstructed from the parent-pointer array?
**Answer:** Starting from the destination node ID, we follow `parent[current]` backward until we reach the source node (whose parent is $-1$). This gives the path in reverse order (Destination $\to \dots \to$ Source). We then reverse the resulting vector to obtain the correct forward path.

### 9. How does Breadth-First Search (BFS) differ from Dijkstra's Algorithm?
**Answer:** BFS explores nodes level by level using a queue, finding the shortest path in terms of **number of edges** (unweighted graphs). Dijkstra's algorithm uses a priority queue/min-heap and edge weights, finding the shortest path by **total weight** (weighted graphs).

### 10. What is the time and space complexity of BFS?
**Answer:** 
- Time Complexity: $O(V + E)$ where $V$ is vertices and $E$ is edges.
- Space Complexity: $O(V)$ for the visited array, queue, and parent pointers.

### 11. How does your Hash Map wrapper assist the graph?
**Answer:** We use `std::unordered_map<int, int>` to map external user-defined node IDs (which might be non-contiguous or sparse, e.g., ID 100, 200, 500) to internal contiguous vector indices ($0, 1, 2 \dots$). This provides $O(1)$ lookup time.

### 12. How does the Binary Min-Heap maintain the heap property?
**Answer:** 
- **Sift-Up (`siftUp`)**: When an element is pushed at the end, it is compared with its parent `(i - 1) / 2`. If smaller, it swaps with the parent and repeats upward until the heap property is restored.
- **Sift-Down (`siftDown`)**: When the root is popped and replaced by the last element, it is compared with its children `2i + 1` and `2i + 2`. It swaps with the smaller child and repeats downward.

### 13. How does SFML render and auto-scale the graph to fit the window?
**Answer:** We compute the bounding box (minimum and maximum $x$ and $y$ coordinates) of all nodes in the graph. We then configure an `sf::View` centered on the graph's bounding center with a zoom factor that fits the graph dimensions into the canvas area (left 1080px of the 1400px window).

### 14. What happens when the destination node is unreachable?
**Answer:** The algorithm completes exploration without reaching the destination. The `PathResult::found` flag remains `false`, `path` is empty, and the UI displays an "Unreachable / Disconnected" warning message.

### 15. What design patterns or principles were used in this project?
**Answer:**
- **Separation of Concerns**: Core data structures (`src/core/`), algorithms (`src/algorithms/`), UI rendering (`src/ui/`), and utilities (`src/utils/`) are strictly isolated.
- **RAII (Resource Acquisition Is Initialization)**: Used for file streams, SFML resources, and memory management.
- **Static Class Methods**: Algorithm classes (`Dijkstra`, `AStar`, `BFS`) use static methods since they operate statelessly on the immutable `Graph` reference.

### 16. How would you scale this application to 10,000+ nodes?
**Answer:** 
- Replace manual vertex iteration in rendering with spatial culling or vertex arrays.
- Implement hierarchical routing (contraction hierarchies).
- Optimize MinHeap with pointer-based index tracking or pairing heaps for faster `decreaseKey`.

### 17. Why SFML instead of SDL or Qt?
**Answer:** SFML provides a modern, object-oriented C++ API, lightweight setup via CMake, robust 2D primitives (`CircleShape`, `VertexArray`), and handles windowing, input events, and timing cleanly without heavy GUI framework bloat.

### 18. Why C++17?
**Answer:** C++17 introduces modern conveniences like `std::optional`, structured bindings, `if constexpr`, string_view, and improved template deduction while remaining widely supported across all major compilers.

### 19. How does your CSV parser handle malformed data?
**Answer:** It uses `std::ifstream` with `std::getline` and `std::istringstream`. If a line is malformed (e.g., missing columns or invalid number formats), it catches exceptions or validates fields, logs a descriptive warning to `stderr`, and skips the line without crashing the application.

### 20. What is RAII and where is it applied?
**Answer:** RAII binds resource lifecycle to object scope. We use it with `std::ifstream` (auto-closed on scope exit), `sf::RenderWindow`, and standard containers (`std::vector`, `std::unordered_map`) which automatically deallocate heap memory on destruction.

### 21. How does the camera pan and zoom work in SFML?
**Answer:** We maintain an `sf::View` object (`m_graphView`). Panning translates the view center (`m_graphView.move(...)`), and zooming scales the view extent (`m_graphView.zoom(...)`), which SFML automatically maps to window coordinates during rendering.

### 22. What testing was performed on the data structures?
**Answer:** 
- Tested MinHeap with random insertions, extractions, and heap property checks.
- Tested Dijkstra and A* against known manual calculations and BFS on unweighted graphs.
- Tested disconnected graphs to verify error handling.

### 23. How do the three weight types work?
**Answer:** We load separate CSV files (`edges_distance.csv`, `edges_time.csv`, `edges_cost.csv`) or allow switching the active edge weight metric, enabling the algorithms to compute shortest paths based on Distance (km), Travel Time (min), or Toll Cost (₹).

### 24. What makes this a "portfolio-grade" project?
**Answer:** Clean folder architecture, zero external STL bypasses for core data structures, comprehensive error handling, live algorithm step-by-step animation, vehicle motion simulation, side-by-side comparison tables, and rigorous documentation (`VIVA_QA.md`).

### 25. How do you defend your code against a professor asking "Why didn't you just use `std::priority_queue`?"
**Answer:** "The course learning objectives for Fundamentals of Data Structures emphasize understanding the internal mechanics of heaps, pointers, and array-based binary trees. Implementing `MinHeap` from scratch with custom sift operations proves mastery of time complexities and heap invariants, whereas `std::priority_queue` is a black-box adapter container."

### 26. What is the purpose of the commented-out disconnected node in `nodes.csv`?
**Answer:** The commented-out node (`# 18,93.94,24.82,Imphal`) serves as a test case for graph connectivity and error handling. It allows developers and evaluators to verify that the graph parser correctly ignores commented lines, and demonstrates how algorithms handle isolated nodes or unreachable components when query tests are performed.
