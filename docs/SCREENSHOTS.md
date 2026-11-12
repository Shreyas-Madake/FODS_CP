# 📸 RouteX — Screenshot Instructions & Gallery Guide

> This guide provides instructions for capturing and embedding screenshots for the RouteX project documentation and presentation reports.

---

## How to Add Screenshots
1. Build and run the RouteX application:
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake --build . --config Release
   ./Release/RouteX.exe  # or bin/RouteX
   ```
2. Take screenshots using the Windows Snipping Tool (`Win + Shift + S`) or in-app capture triggers.
3. Save the captured images as PNG files in this `docs/` folder with the exact filenames listed below.
4. Ensure images are crisp (minimum 1280x720 resolution recommended) showing the SFML canvas and side panel.

---

## Expected Screenshots & Descriptions

### 1. Initial State (`initial_state.png`)
* **Description:** Application launch showing the Indian city network graph rendered on the SFML canvas with nodes, city name labels, and bounding box auto-scaling.
* **Key Elements:** Delhi, Mumbai, Bangalore, Kolkata nodes visible with clean anti-aliased circles.

### 2. Source & Destination Selected (`selection.png`)
* **Description:** Selection state where Delhi is chosen as the source node (highlighted in Green) and Chennai as the destination node (highlighted in Red).
* **Key Elements:** Side panel reflecting selected node coordinates and IDs.

### 3. Algorithm Animation (`animation.png`)
* **Description:** Live step-by-step visualization of Dijkstra's algorithm exploring the network.
* **Key Elements:** Visited nodes shown in green, currently expanding node highlighted in yellow, exploration wavefront expanding across central India.

### 4. Shortest Path Found (`shortest_path.png`)
* **Description:** Final shortest path successfully computed and highlighted in bold red across the network.
* **Key Elements:** Side panel displaying total distance (km), estimated travel time (min), and toll cost (₹).

### 5. Vehicle Animation (`vehicle.png`)
* **Description:** Vehicle simulation mode active.
* **Key Elements:** A smooth glowing cyan dot traveling along the computed shortest path from source to destination with interpolation.

### 6. Algorithm Comparison (`comparison.png`)
* **Description:** Side-by-side performance comparison view.
* **Key Elements:** Table and stats comparing Dijkstra (optimal weighted), A* (optimal heuristic search), and BFS (edge count) in nodes visited and execution time (microseconds).
