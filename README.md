# Smart Route Planner

**A modular C++ application for graph algorithms, shortest-path computation, road-network management, and performance benchmarking.**

Smart Route Planner is a console-based application developed in modern C++ to explore and compare graph algorithms through practical route-planning scenarios.

The project supports weighted undirected road networks, a separate directed graph mode, interactive graph editing, CSV file persistence, automated testing, and algorithm benchmarking.

## Features

- Interactive command-line interface for managing road networks.
- Shortest-path computation using Dijkstra, A*, Bellman–Ford, and Floyd–Warshall.
- Minimum-hop route discovery using Breadth-First Search (BFS).
- Minimum spanning tree computation using Kruskal's algorithm.
- Separate directed graph mode supporting negative-weight edges and negative-cycle detection.
- Add, update, and remove roads.
- Save and load undirected road networks using CSV files.
- Support for disconnected graphs and parallel roads.
- Reusable input-validation utilities.
- Automated unit tests and randomized algorithm-consistency testing.
- Reproducible performance benchmarks with CSV export.

## Implemented Algorithms

| Algorithm | Purpose | Typical time complexity |
|---|---|---|
| Dijkstra | Single-source shortest paths with nonnegative weights | O((V + E) log V) |
| A* | Heuristic-guided shortest-path search | Depends on the heuristic and explored states |
| BFS | Shortest path by number of edges | O(V + E) |
| Bellman–Ford | Shortest paths with negative-weight support in directed mode | O(VE) |
| Floyd–Warshall | All-pairs shortest paths | O(V³) |
| Kruskal | Minimum spanning tree or forest | O(E log E) |

Here, **V** represents the number of vertices and **E** the number of edges.

### Dijkstra and A*

Dijkstra calculates minimum-cost routes for graphs with nonnegative edge weights.

A* extends shortest-path search with a heuristic that estimates the remaining cost to the destination.

The road-network implementation uses Euclidean distance as its heuristic. Road weights are validated against straight-line distances to maintain an admissible heuristic for the supported network model.

### Breadth-First Search

BFS finds a route containing the minimum number of road connections.

Unlike Dijkstra, BFS does not minimize the sum of weighted road distances.

### Bellman–Ford

Bellman–Ford repeatedly relaxes edges to calculate shortest-path distances.

The separate directed graph implementation supports negative edge weights and checks whether a reachable negative-weight cycle affects the requested destination.

### Floyd–Warshall

Floyd–Warshall computes shortest-path distances between all pairs of vertices using dynamic programming.

Its O(V³) time complexity and O(V²) storage requirements make it more appropriate for relatively small graphs.

### Kruskal

Kruskal computes a minimum spanning tree using a greedy strategy and Disjoint Set Union (Union-Find).

For disconnected networks, the implementation returns a minimum spanning forest.

A minimum spanning tree minimizes the combined weight of selected connections; it does not necessarily provide the shortest route between arbitrary cities.

## Project Architecture

```text
Smart Route Planner/
├── include/
│   ├── Graph.h
│   ├── DirectedGraph.h
│   ├── DirectedGraphMenu.h
│   ├── InputUtils.h
│   ├── NetworkIO.h
│   └── algorithms/
│       ├── Dijkstra.h
│       ├── AStar.h
│       ├── BFS.h
│       ├── BellmanFord.h
│       ├── FloydWarshall.h
│       ├── Kruskal.h
│       └── DirectedBellmanFord.h
├── src/
│   ├── main.cpp
│   ├── Graph.cpp
│   ├── DirectedGraph.cpp
│   ├── DirectedGraphMenu.cpp
│   ├── InputUtils.cpp
│   ├── NetworkIO.cpp
│   └── algorithms/
│       ├── Dijkstra.cpp
│       ├── AStar.cpp
│       ├── BFS.cpp
│       ├── BellmanFord.cpp
│       ├── FloydWarshall.cpp
│       ├── Kruskal.cpp
│       └── DirectedBellmanFord.cpp
├── tests/
│   ├── test_graph.cpp
│   ├── test_consistency.cpp
│   └── test_input.cpp
├── benchmarks/
│   └── benchmark.cpp
├── data/
├── CMakeLists.txt
├── .gitignore
└── README.md
```

### Design

The application separates graph storage, algorithm implementations, input processing, file operations, and interactive menus.

`Graph` manages the undirected road network using adjacency lists and city coordinates.

`DirectedGraph` manages directed weighted edges independently of the physical road-network model.

The algorithm implementations are located in dedicated modules and return structured results.

`NetworkIO` handles CSV serialization and deserialization. CSV loading builds a temporary graph and replaces the active network only after validation succeeds.

`InputUtils` centralizes console input validation and provides functions that can also be tested using standard C++ streams.

## Requirements

- A C++17-compatible compiler
- CMake 3.16 or newer
- A supported build tool, such as MinGW Make
- Windows PowerShell for the commands shown below

The project was developed using Visual Studio Code and the MSYS2 UCRT64 MinGW toolchain on Windows.

## Build Instructions

Open PowerShell in the project's root directory.

Configure a Release build:

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

Compile:

```powershell
cmake --build build --parallel
```

Run the interactive application:

```powershell
.\build\route_planner.exe
```

If CMake is installed but unavailable through the system PATH, invoke it using its installed executable path.

## Using the Application

The main menu provides operations for:

- Viewing the road network.
- Calculating routes with different algorithms.
- Adding cities and roads.
- Editing or deleting roads.
- Computing minimum spanning trees.
- Entering directed graph mode.
- Saving and loading road networks.

Select a menu option and follow the displayed prompts.

Choose `0` to exit the main application.

Inside Directed Graph Mode, `0` returns to the main menu.

### CSV Persistence

The application supports a CSV representation containing city coordinates and weighted roads.

Example:

```csv
type,field1,field2,field3
CITY,"A",0,0
CITY,"B",1,0
CITY,"C",2,0
ROAD,"A","B",4
ROAD,"B","C",3
```

The interactive application saves and loads network files from the `data/` directory when launched from the project root.

Loading a valid network replaces the current undirected road network. Invalid CSV input is rejected without partially replacing the existing network.

## Automated Testing

The project uses CTest to execute three testing components:

| Test executable | Focus |
|---|---|
| `route_tests` | Graph operations, algorithms, file persistence, and edge cases |
| `route_consistency_tests` | Cross-algorithm consistency across generated graphs |
| `route_input_tests` | Console input validation and recovery |

Run all registered tests:

```powershell
ctest --test-dir build --output-on-failure
```

Run an individual test executable:

```powershell
.\build\route_tests.exe
```

The unit-test suite covers graph connectivity, shortest-path computation, road editing, CSV validation, negative-cycle behavior, and other edge cases.

The consistency suite generates 30 deterministic road networks with eight cities each and compares shortest-path results across algorithms for every ordered pair, producing 1,920 route comparisons.

Using a fixed random seed makes generated test cases reproducible.

## Performance Benchmarking

The benchmarking program evaluates algorithms on reproducible square-grid networks.

| Grid size | Vertices | Undirected edges |
|---|---:|---:|
| 10 × 10 | 100 | 180 |
| 20 × 20 | 400 | 760 |
| 30 × 30 | 900 | 1,740 |
| 40 × 40 | 1,600 | 3,120 |

The benchmark measures execution time using `std::chrono::steady_clock`.

It distinguishes between weighted shortest-path algorithms, BFS minimum-hop searches, Kruskal's spanning-tree computation, and Floyd–Warshall's preprocessing and query operations.

Floyd–Warshall is benchmarked only on the smaller graph sizes to keep preprocessing costs manageable.

Run the benchmark:

```powershell
.\build\route_benchmark.exe
```

Results are exported to:

```text
data/benchmark_results.csv
```

The CSV contains graph sizes, algorithm names, query counts, execution times, and relevant route metrics.

### Experimental Performance Results

Performance measurements were collected using the project's C++17 benchmark suite on reproducible weighted grid networks containing 100, 400, 900, and 1,600 vertices.

The benchmark uses `std::chrono::steady_clock` and exports its results to `data/benchmark_results.csv`.

**Average shortest-path query time (milliseconds)**

| Vertices | Dijkstra | A* | Bellman–Ford |
|---|---:|---:|---:|
| 100 | 0.0546 | 0.0669 | 0.4223 |
| 400 | 0.3127 | 0.2705 | 2.5954 |
| 900 | 0.5733 | 0.4894 | 7.3668 |
| 1,600 | 0.9374 | 1.0317 | 20.5561 |

#### Key Observations

**Dijkstra vs. A\***

A* achieved approximately 13.5% and 14.6% lower average query times than Dijkstra on the 400-vertex and 900-vertex networks, respectively. However, Dijkstra was faster on the 100-vertex and 1,600-vertex networks.

This illustrates how heuristic-guided search performance depends on the graph structure, search queries, heuristic effectiveness, and implementation overhead.

**Bellman–Ford**

Bellman–Ford required substantially more execution time than Dijkstra or A* across all evaluated network sizes. At 1,600 vertices, its average query time was approximately 20.56 ms, compared with 0.94 ms for Dijkstra.

The additional computational cost is associated with repeated edge relaxation. Bellman–Ford remains valuable for directed graphs containing negative edge weights.

**Floyd–Warshall**

Floyd–Warshall demonstrated a trade-off between preprocessing and query performance.

| Vertices | Preprocessing (ms) | Average query (ms) |
|---|---:|---:|
| 100 | 1.2603 | 0.001515 |
| 400 | 37.0502 | 0.003870 |

Although preprocessing is more expensive, subsequent shortest-path queries are fast because the distance information has already been computed.

**BFS and Kruskal**

BFS produced minimum-hop routes rather than minimum-weight routes. Kruskal constructed minimum spanning trees for the connected benchmark networks.

These algorithms solve different optimization problems and are therefore evaluated separately from weighted shortest-path algorithms.

#### Benchmark Limitations

The measurements were collected from deterministic grid networks, not real-world geographical datasets. The number of queries varies with graph size, and each configuration was measured using a single timing batch.

Consequently, the results describe observed behavior in this benchmark rather than statistically established performance guarantees.

Future

### Benchmark Interpretation

Benchmark measurements depend on the compiler, optimization settings, hardware, and system load.

The generated measurements should be interpreted alongside the theoretical complexity of each algorithm.

BFS and Kruskal solve different optimization problems from weighted shortest-path algorithms, so their results should not be compared as though they calculate equivalent outputs.

Floyd–Warshall's preprocessing cost must also be considered separately from the cost of querying its precomputed distance matrix.

## Engineering Concepts Demonstrated

This project provides practical experience with:

- Modern C++17
- Object-oriented design and encapsulation
- Graph data structures
- Greedy algorithms
- Dynamic programming
- Heuristic search
- Disjoint Set Union
- Algorithmic complexity analysis
- Modular project architecture
- CMake and CTest
- File serialization and input validation
- Automated testing and reproducible benchmarking

## Current Scope and Limitations

- The application uses a console-based interface.
- Undirected roads use nonnegative integer weights.
- Directed graphs are managed separately from the standard road network.
- Floyd–Warshall has cubic computational complexity.
- The benchmark reports measured results for generated grid networks rather than real geographical datasets.
- CSV data persistence currently targets the undirected road network.

## Future Improvements

Possible extensions include graphical network visualization, additional network-file formats, more extensive benchmark statistics, directed-graph persistence, and automated continuous integration.

## License

This project is licensed under the MIT License.

See the [LICENSE](LICENSE) file for details.

## 🤝 Connect With Me

I'm always interested in learning new technologies, discussing software engineering, and connecting with fellow developers.

Feel free to explore my projects or reach out!

- **GitHub:** [asadaliprofile](https://github.com/asadaliprofile)
- **LinkedIn:** [asadaliprofile](https://www.linkedin.com/in/asadaliprofile/)

If you found this project interesting, feel free to ⭐ the repository!

---

*Built with C++, curiosity, and a commitment to continuous learning.*