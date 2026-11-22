
#include "Graph.h"
#include "algorithms/BellmanFord.h"
#include "algorithms/FloydWarshall.h"
#include "algorithms/Kruskal.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

int main() {
    const int graphCount = 30;
    const int cityCount = 8;

    mt19937 rng(2026);
    uniform_int_distribution<int> weightDist(1, 20);
    uniform_int_distribution<int> cityDist(0, cityCount - 1);

    int comparisons = 0;
    int failures = 0;

    for (int test = 0; test < graphCount; ++test) {
        Graph graph;
        vector<string> names;

        // Generate cities at known planar coordinates.
        for (int i = 0; i < cityCount; ++i) {
            string name = "City" + to_string(i);
            names.push_back(name);

            graph.addCity(name, i, 0);
        }

        // First create a connected chain.
        for (int i = 0; i < cityCount - 1; ++i) {
            graph.addRoad(
                names[i],
                names[i + 1],
                weightDist(rng)
            );
        }

        // Add additional randomly weighted roads.
        for (int i = 0; i < 15; ++i) {
            int a = cityDist(rng);
            int b = cityDist(rng);

            if (a == b)
                continue;

            // Preserve the straight-line lower bound
            // required for A* optimality.
            int minimumDistance = abs(a - b);
            int weight = max(minimumDistance, weightDist(rng));

            graph.addRoad(names[a], names[b], weight);
        }

        // Precompute all-pairs shortest paths once.
        AllPairsResult matrix = floydWarshall(graph);

        if (matrix.negativeCycle) {
            cerr << "[FAIL] Unexpected negative cycle\n";
            ++failures;
            continue;
        }

        // Compare algorithms for every ordered city pair.
        for (const string& start : names) {
            for (const string& destination : names) {
                RouteResult dijkstra =
                    graph.calculateRoute(start, destination, false);

                RouteResult astar =
                    graph.calculateRoute(start, destination, true);

                RouteResult bellman =
                    bellmanFordShortestPath(
                        graph, start, destination
                    );

                RouteResult floyd =
                    getFloydWarshallRoute(
                        matrix, start, destination
                    );

                bool sameDistance =
                    dijkstra.distance == astar.distance &&
                    dijkstra.distance == bellman.distance &&
                    dijkstra.distance == floyd.distance;

                bool allFound =
                    dijkstra.found &&
                    astar.found &&
                    bellman.found &&
                    floyd.found;

                bool pathsValid = allFound;

                for (const RouteResult* result :
                     {&dijkstra, &astar, &bellman, &floyd}) {

                    pathsValid =
                        pathsValid &&
                        !result->path.empty() &&
                        result->path.front() == start &&
                        result->path.back() == destination;
                }

                if (!sameDistance ||
                    !allFound ||
                    !pathsValid) {

                    cerr << "[FAIL] Graph " << test
                         << ": " << start
                         << " -> " << destination
                         << '\n';

                    ++failures;
                }

                ++comparisons;
            }
        }

        // Verify Kruskal's spanning tree properties.
        MSTResult mst = kruskalMST(graph);

        if (!mst.connected ||
            mst.edges.size() !=
                static_cast<size_t>(cityCount - 1) ||
            mst.components != 1) {

            cerr << "[FAIL] Kruskal on graph "
                 << test << '\n';

            ++failures;
        }
    }

    cout << "\n===== ALGORITHM CONSISTENCY TEST =====\n";
    cout << "Generated graphs: " << graphCount << '\n';
    cout << "Shortest-path comparisons: "
         << comparisons << '\n';
    cout << "Failures: " << failures << '\n';

    if (failures == 0) {
        cout << "[PASS] All consistency checks passed!\n";
    }

    return failures == 0 ? 0 : 1;
}
