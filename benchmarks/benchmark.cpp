
#include "Graph.h"
#include "algorithms/BFS.h"
#include "algorithms/BellmanFord.h"
#include "algorithms/FloydWarshall.h"
#include "algorithms/Kruskal.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

using Clock = chrono::steady_clock;

struct BenchmarkCase {
    int side;
    int queries;
};

struct BenchmarkRow {
    int vertices;
    int roads;
    int queries;
    string algorithm;
    double totalMs;
    double averageMs;
    double averageCost;
    double averageHops;
};

struct Query {
    string start;
    string destination;
};

string cityName(int row, int col, int side) {
    return "City" + to_string(row * side + col);
}

// Generate a connected grid with reproducible weights.
Graph createGrid(int side, unsigned int seed) {
    Graph graph;

    mt19937 rng(seed);
    uniform_int_distribution<int> weightDist(1, 10);

    for (int row = 0; row < side; ++row) {
        for (int col = 0; col < side; ++col) {
            graph.addCity(
                cityName(row, col, side),
                static_cast<double>(col),
                static_cast<double>(row)
            );
        }
    }

    for (int row = 0; row < side; ++row) {
        for (int col = 0; col < side; ++col) {
            string current = cityName(row, col, side);

            if (col + 1 < side) {
                graph.addRoad(
                    current,
                    cityName(row, col + 1, side),
                    weightDist(rng)
                );
            }

            if (row + 1 < side) {
                graph.addRoad(
                    current,
                    cityName(row + 1, col, side),
                    weightDist(rng)
                );
            }
        }
    }

    return graph;
}

// Generate the same queries each time the benchmark runs.
vector<Query> generateQueries(
    int side,
    int count,
    unsigned int seed
) {
    mt19937 rng(seed);

    const int vertexCount = side * side;

    uniform_int_distribution<int> vertexDist(
        0, vertexCount - 1
    );

    vector<Query> queries;
    queries.reserve(count);

    for (int i = 0; i < count; ++i) {
        int a = vertexDist(rng);
        int b = vertexDist(rng);

        while (a == b)
            b = vertexDist(rng);

        queries.push_back({
            "City" + to_string(a),
            "City" + to_string(b)
        });
    }

    return queries;
}

template <typename Function>
double measureMilliseconds(Function operation) {
    auto start = Clock::now();

    operation();

    auto finish = Clock::now();

    return chrono::duration<double, milli>(
        finish - start
    ).count();
}

// Measure route queries, keeping their results
// for correctness checks after timing.
template <typename Function>
BenchmarkRow measureRoutes(
    int vertices,
    int roads,
    const string& algorithm,
    const vector<Query>& queries,
    Function routeFunction,
    vector<RouteResult>& results
) {
    results.clear();
    results.reserve(queries.size());

    double elapsed = measureMilliseconds([&]() {
        for (const auto& query : queries) {
            results.push_back(
                routeFunction(
                    query.start,
                    query.destination
                )
            );
        }
    });

    double totalCost = 0.0;
    double totalHops = 0.0;

    for (const auto& result : results) {
        if (!result.validCities ||
            !result.found ||
            result.negativeCycle ||
            result.path.empty()) {
            throw runtime_error(
                algorithm + " failed to find a route."
            );
        }

        totalCost += static_cast<double>(
            result.distance
        );

        totalHops += static_cast<double>(
            result.path.size() - 1
        );
    }

    const double count =
        static_cast<double>(queries.size());

    return {
        vertices,
        roads,
        static_cast<int>(queries.size()),
        algorithm,
        elapsed,
        elapsed / count,
        totalCost / count,
        totalHops / count
    };
}

void verifyShortestPaths(
    const vector<RouteResult>& reference,
    const vector<RouteResult>& other,
    const vector<Query>& queries,
    const string& algorithm
) {
    if (reference.size() != other.size())
        throw runtime_error("Result count mismatch.");

    for (size_t i = 0; i < reference.size(); ++i) {
        if (reference[i].distance != other[i].distance ||
            reference[i].found != other[i].found) {
            throw runtime_error(
                "Incorrect shortest path in " +
                algorithm + ": " +
                queries[i].start + " -> " +
                queries[i].destination
            );
        }
    }
}

void printRow(const BenchmarkRow& row) {
    cout << left
         << setw(22) << row.algorithm
         << right
         << setw(13) << fixed << setprecision(3)
         << row.totalMs
         << setw(13) << row.averageMs
         << '\n';
}

void writeCsv(
    const vector<BenchmarkRow>& rows,
    const string& filename
) {
    fs::create_directories("data");

    ofstream file(filename);

    if (!file)
        throw runtime_error(
            "Cannot create benchmark CSV file."
        );

    file << "vertices,roads,queries,algorithm,"
         << "total_ms,average_ms,average_cost,"
         << "average_hops\n";

    file << fixed << setprecision(6);

    for (const auto& row : rows) {
        file << row.vertices << ','
             << row.roads << ','
             << row.queries << ','
             << row.algorithm << ','
             << row.totalMs << ','
             << row.averageMs << ','
             << row.averageCost << ','
             << row.averageHops << '\n';
    }

    file.flush();

    if (!file)
        throw runtime_error(
            "Failed to write benchmark results."
        );
}

int main() {
    try {
        // Larger graphs use fewer queries because
        // Bellman-Ford can be computationally expensive.
        const vector<BenchmarkCase> cases = {
            {10, 40},
            {20, 20},
            {30, 12},
            {40, 8}
        };

        vector<BenchmarkRow> allRows;

        cout << "\n===== ROUTE PLANNER BENCHMARK =====\n";
        cout << "Compiler optimization: Release recommended\n";
        cout << "Graph generation seed: 2026\n";

        for (const auto& config : cases) {
            const int side = config.side;
            const int vertices = side * side;
            const int roads = 2 * side * (side - 1);

            Graph graph = createGrid(side, 2026);

            vector<Query> queries = generateQueries(
                side,
                config.queries,
                1000 + static_cast<unsigned int>(side)
            );

            cout << "\n====================================\n";
            cout << "Vertices: " << vertices << '\n';
            cout << "Roads: " << roads << '\n';
            cout << "Route queries: " << queries.size() << '\n';
            cout << "====================================\n";

            cout << left << setw(22) << "Algorithm"
                 << right << setw(13) << "Total (ms)"
                 << setw(13) << "Avg (ms)"
                 << '\n';

            vector<RouteResult> dijkstraResults;
            vector<RouteResult> astarResults;
            vector<RouteResult> bellmanResults;
            vector<RouteResult> bfsResults;

            // Dijkstra provides our weighted-path reference.
            auto dijkstraRow = measureRoutes(
                vertices, roads, "Dijkstra",
                queries,
                [&](const string& a, const string& b) {
                    return graph.calculateRoute(a, b, false);
                },
                dijkstraResults
            );

            allRows.push_back(dijkstraRow);
            printRow(dijkstraRow);

            // A* should agree with Dijkstra on distance.
            auto astarRow = measureRoutes(
                vertices, roads, "AStar",
                queries,
                [&](const string& a, const string& b) {
                    return graph.calculateRoute(a, b, true);
                },
                astarResults
            );

            verifyShortestPaths(
                dijkstraResults, astarResults,
                queries, "AStar"
            );

            allRows.push_back(astarRow);
            printRow(astarRow);

            // Bellman-Ford also minimizes weighted cost.
            auto bellmanRow = measureRoutes(
                vertices, roads, "BellmanFord",
                queries,
                [&](const string& a, const string& b) {
                    return bellmanFordShortestPath(
                        graph, a, b
                    );
                },
                bellmanResults
            );

            verifyShortestPaths(
                dijkstraResults, bellmanResults,
                queries, "BellmanFord"
            );

            allRows.push_back(bellmanRow);
            printRow(bellmanRow);

            // BFS minimizes hops, not weighted distance.
            auto bfsRow = measureRoutes(
                vertices, roads, "BFS",
                queries,
                [&](const string& a, const string& b) {
                    return breadthFirstSearch(graph, a, b);
                },
                bfsResults
            );

            allRows.push_back(bfsRow);
            printRow(bfsRow);

            // Floyd-Warshall uses O(V^3) preprocessing.
            // Restrict it to the smaller graph sizes.
            if (vertices <= 400) {
                AllPairsResult matrix;

                double preprocessingMs =
                    measureMilliseconds([&]() {
                        matrix = floydWarshall(graph);
                    });

                if (matrix.negativeCycle) {
                    throw runtime_error(
                        "Unexpected negative cycle."
                    );
                }

                allRows.push_back({
                    vertices,
                    roads,
                    1,
                    "FloydWarshallPrecompute",
                    preprocessingMs,
                    preprocessingMs,
                    0.0,
                    0.0
                });

                vector<RouteResult> floydResults;

                auto floydQueryRow = measureRoutes(
                    vertices, roads,
                    "FloydWarshallQuery",
                    queries,
                    [&](const string& a, const string& b) {
                        return getFloydWarshallRoute(
                            matrix, a, b
                        );
                    },
                    floydResults
                );

                verifyShortestPaths(
                    dijkstraResults, floydResults,
                    queries, "FloydWarshall"
                );

                printRow(allRows.back());
                allRows.push_back(floydQueryRow);
                printRow(floydQueryRow);
            }

            // Kruskal computes an MST, not a route.
            MSTResult mst;

            double mstMs = measureMilliseconds([&]() {
                mst = kruskalMST(graph);
            });

            if (!mst.connected ||
                mst.edges.size() !=
                    static_cast<size_t>(vertices - 1)) {
                throw runtime_error(
                    "Kruskal produced an invalid tree."
                );
            }

            BenchmarkRow mstRow = {
                vertices, roads, 1,
                "KruskalMST",
                mstMs, mstMs,
                static_cast<double>(mst.totalWeight),
                static_cast<double>(mst.edges.size())
            };

            allRows.push_back(mstRow);
            printRow(mstRow);

            cout << "Weighted shortest-path checks: PASS\n";
            cout << "Kruskal connectivity check: PASS\n";
        }

        const string filename =
            "data/benchmark_results.csv";

        writeCsv(allRows, filename);

        cout << "\n===== BENCHMARK COMPLETE =====\n";
        cout << "Results saved to: " << filename << '\n';
        cout << "All correctness checks passed.\n";

        return 0;
    }
    catch (const exception& e) {
        cerr << "\nBenchmark error: "
             << e.what() << '\n';

        return 1;
    }
}
