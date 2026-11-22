#include "algorithms/BFS.h"
#include "Graph.h"
#include "algorithms/BellmanFord.h"
#include "algorithms/FloydWarshall.h"
#include "algorithms/Kruskal.h"
#include "algorithms/DirectedBellmanFord.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "NetworkIO.h"
#include <fstream>
#include <cstdio>
#include <string>
#include <exception>

#include <limits>

using namespace std;

int passed = 0;
int failed = 0;

void check(bool condition, const string& testName) {
    if (condition) {
        cout << "[PASS] " << testName << '\n';
        ++passed;
    } else {
        cerr << "[FAIL] " << testName << '\n';
        ++failed;
    }
}

template <typename Function>
bool throwsInvalidArgument(Function action) {
    try {
        action();
    }
    catch (const invalid_argument&) {
        return true;
    }

    return false;
}

int main() {
    Graph graph;

    graph.addCity("A", 0, 0);
    graph.addCity("B", 0.5, 0);
    graph.addCity("C", 0, 0.5);
    graph.addCity("D", 0.5, 0.5);

    graph.addRoad("A", "B", 4);
    graph.addRoad("A", "C", 2);
    graph.addRoad("B", "D", 3);
    graph.addRoad("C", "D", 3);
    graph.addRoad("B", "C", 1);

    vector<string> expectedPath = {"A", "C", "D"};

    // Test 1: Dijkstra's algorithm
    RouteResult dijkstra = graph.calculateRoute("A", "D", false);

    check(
        dijkstra.validCities &&
        dijkstra.found &&
        dijkstra.distance == 5 &&
        dijkstra.path == expectedPath,
        "Dijkstra finds the correct shortest route"
    );

    // Test 2: A* algorithm
    RouteResult astar = graph.calculateRoute("A", "D", true);

    check(
        astar.validCities &&
        astar.found &&
        astar.distance == 5 &&
        astar.path == expectedPath,
        "A* finds the correct shortest route"
    );

    // Test 3: Algorithm agreement
    check(
        dijkstra.distance == astar.distance,
        "Dijkstra and A* agree on shortest distance"
    );

    // Test 4: Starting city equals destination
    RouteResult same = graph.calculateRoute("A", "A", false);

    check(
        same.found &&
        same.distance == 0 &&
        same.path == vector<string>{"A"},
        "Same start and destination"
    );

    // Test 5: Unknown destination
    RouteResult unknown = graph.calculateRoute("A", "Z", false);

    check(
        !unknown.validCities && !unknown.found,
        "Unknown city is handled"
    );

    // Test 6: Disconnected city
    graph.addCity("E", 10, 10);

    RouteResult disconnected = graph.calculateRoute("A", "E", true);

    check(
        disconnected.validCities && !disconnected.found,
        "Disconnected city is handled"
    );

    // Test 7: Negative road distance
    check(
        throwsInvalidArgument([&]() {
            graph.addRoad("A", "B", -5);
        }),
        "Negative road distance is rejected"
    );

    // Test 8: Duplicate city
    check(
        throwsInvalidArgument([&]() {
            graph.addCity("A", 1, 1);
        }),
        "Duplicate city is rejected"
    );

    // Test 9: Nonexistent endpoint
    check(
        throwsInvalidArgument([&]() {
            graph.addRoad("A", "Z", 5);
        }),
        "Road with nonexistent city is rejected"
    );

    // Test 10: Impossible road distance
    check(
        throwsInvalidArgument([&]() {
            graph.addRoad("A", "E", 1);
        }),
        "Road shorter than straight-line distance is rejected"
    );


    // Test 11: BFS start equals destination.
    RouteResult bfsSame =
        breadthFirstSearch(graph, "A", "A");

    check(
        bfsSame.validCities &&
        bfsSame.found &&
        bfsSame.distance == 0 &&
        bfsSame.path == vector<string>{"A"},
        "BFS handles identical start and destination"
    );

    // Test 12: BFS handles unknown cities.
    RouteResult bfsUnknown =
        breadthFirstSearch(graph, "A", "Unknown");

    check(
        !bfsUnknown.validCities && !bfsUnknown.found,
        "BFS handles unknown cities"
    );

    // Test 13: BFS minimizes number of roads.
    Graph comparison;

    comparison.addCity("A", 0, 0);
    comparison.addCity("B", 0, 0);
    comparison.addCity("C", 0, 0);
    comparison.addCity("D", 0, 0);
    comparison.addCity("E", 0, 0);

    comparison.addRoad("A", "B", 10);
    comparison.addRoad("B", "D", 10);

    comparison.addRoad("A", "C", 1);
    comparison.addRoad("C", "E", 1);
    comparison.addRoad("E", "D", 1);

    RouteResult bfs =
        breadthFirstSearch(comparison, "A", "D");

    RouteResult shortest =
        comparison.calculateRoute("A", "D", false);

    check(
        bfs.found &&
        bfs.path == vector<string>({"A", "B", "D"}) &&
        bfs.distance == 20 &&
        shortest.found &&
        shortest.distance == 3,
        "BFS minimizes road count, not distance"
    );


    // Test 14: Bellman-Ford shortest route.
    RouteResult bellman =
        bellmanFordShortestPath(graph, "A", "D");

    check(
        bellman.validCities &&
        bellman.found &&
        !bellman.negativeCycle &&
        bellman.distance == 5 &&
        bellman.path == vector<string>({"A", "C", "D"}),
        "Bellman-Ford finds correct shortest route"
    );

    // Test 15: Bellman-Ford agrees with Dijkstra.
    RouteResult dij =
        graph.calculateRoute("A", "D", false);

    check(
        bellman.distance == dij.distance,
        "Bellman-Ford and Dijkstra agree"
    );

    // Test 16: Unknown city.
    RouteResult bfUnknown =
        bellmanFordShortestPath(graph, "A", "Unknown");

    check(
        !bfUnknown.validCities &&
        !bfUnknown.found,
        "Bellman-Ford handles unknown cities"
    );

    // Test 17: Disconnected city E.
    RouteResult bfDisconnected =
        bellmanFordShortestPath(graph, "A", "E");

    check(
        bfDisconnected.validCities &&
        !bfDisconnected.found &&
        !bfDisconnected.negativeCycle,
        "Bellman-Ford handles disconnected cities"
    );

    // Test 18: Same start and destination.
    RouteResult bfSame =
        bellmanFordShortestPath(graph, "A", "A");

    check(
        bfSame.found &&
        bfSame.distance == 0 &&
        bfSame.path == vector<string>{"A"},
        "Bellman-Ford handles identical endpoints"
    );


    // Test 19: Floyd-Warshall finds the correct route.
    AllPairsResult fwMatrix = floydWarshall(graph);

    RouteResult fwRoute =
        getFloydWarshallRoute(fwMatrix, "A", "D");

    check(
        fwRoute.validCities &&
        fwRoute.found &&
        fwRoute.distance == 5 &&
        fwRoute.path == vector<string>({"A", "C", "D"}),
        "Floyd-Warshall finds correct shortest route"
    );

    // Test 20: Floyd-Warshall agrees with Dijkstra.
    RouteResult dijResult =
        graph.calculateRoute("A", "D", false);

    check(
        fwRoute.found &&
        dijResult.found &&
        fwRoute.distance == dijResult.distance,
        "Floyd-Warshall agrees with Dijkstra"
    );

    // Test 21: Same start and destination.
    RouteResult fwSame =
        getFloydWarshallRoute(fwMatrix, "A", "A");

    check(
        fwSame.found &&
        fwSame.distance == 0 &&
        fwSame.path == vector<string>{"A"},
        "Floyd-Warshall handles identical endpoints"
    );

    // Test 22: Unknown city.
    RouteResult fwUnknown =
        getFloydWarshallRoute(fwMatrix, "A", "Unknown");

    check(
        !fwUnknown.validCities &&
        !fwUnknown.found,
        "Floyd-Warshall handles unknown city"
    );

    // Test 23: Disconnected city.
    RouteResult fwDisconnected =
        getFloydWarshallRoute(fwMatrix, "A", "E");

    check(
        fwDisconnected.validCities &&
        !fwDisconnected.found,
        "Floyd-Warshall handles disconnected city"
    );


    // Test 24: Correct minimum spanning tree.
    Graph mstGraph;

    mstGraph.addCity("A", 0, 0);
    mstGraph.addCity("B", 0, 0);
    mstGraph.addCity("C", 0, 0);
    mstGraph.addCity("D", 0, 0);

    mstGraph.addRoad("A", "B", 4);
    mstGraph.addRoad("A", "C", 2);
    mstGraph.addRoad("B", "D", 3);
    mstGraph.addRoad("C", "D", 3);
    mstGraph.addRoad("B", "C", 1);

    MSTResult mst = kruskalMST(mstGraph);

    check(
        mst.connected &&
        mst.components == 1 &&
        mst.totalWeight == 6 &&
        mst.edges.size() == 3,
        "Kruskal finds the correct MST"
    );

    // Test 25: Disconnected network.
    Graph disconnectedMST;

    disconnectedMST.addCity("A", 0, 0);
    disconnectedMST.addCity("B", 0, 0);
    disconnectedMST.addCity("C", 0, 0);

    disconnectedMST.addRoad("A", "B", 2);

    MSTResult forest = kruskalMST(disconnectedMST);

    check(
        !forest.connected &&
        forest.components == 2 &&
        forest.edges.size() == 1 &&
        forest.totalWeight == 2,
        "Kruskal handles disconnected graphs"
    );

    // Test 26: Single city.
    Graph singleCityGraph;

    singleCityGraph.addCity("A", 0, 0);

    MSTResult single = kruskalMST(singleCityGraph);

    check(
        single.connected &&
        single.components == 1 &&
        single.edges.empty() &&
        single.totalWeight == 0,
        "Kruskal handles a single city"
    );

    // Test 27: Parallel roads.
    Graph parallelGraph;

    parallelGraph.addCity("A", 0, 0);
    parallelGraph.addCity("B", 0, 0);

    parallelGraph.addRoad("A", "B", 10);
    parallelGraph.addRoad("A", "B", 3);

    MSTResult parallel = kruskalMST(parallelGraph);

    check(
        parallel.connected &&
        parallel.edges.size() == 1 &&
        parallel.totalWeight == 3,
        "Kruskal selects the cheapest parallel road"
    );

    // Test 28: Empty graph.
    Graph emptyGraph;

    MSTResult empty = kruskalMST(emptyGraph);

    check(
        empty.connected &&
        empty.components == 0 &&
        empty.edges.empty() &&
        empty.totalWeight == 0,
        "Kruskal handles an empty graph"
    );


    // Test 29: Negative edge without a negative cycle.
    DirectedGraph directed;

    directed.addVertex("A");
    directed.addVertex("B");
    directed.addVertex("C");

    directed.addEdge("A", "B", 4);
    directed.addEdge("B", "C", -2);
    directed.addEdge("A", "C", 10);

    RouteResult negativeEdge =
        directedBellmanFord(directed, "A", "C");

    check(
        negativeEdge.validCities &&
        negativeEdge.found &&
        !negativeEdge.negativeCycle &&
        negativeEdge.distance == 2 &&
        negativeEdge.path == vector<string>({"A", "B", "C"}),
        "Directed Bellman-Ford handles negative weights"
    );

    // Test 30: Detect a negative cycle affecting destination.
    DirectedGraph cycleGraph;

    cycleGraph.addVertex("A");
    cycleGraph.addVertex("B");
    cycleGraph.addVertex("C");
    cycleGraph.addVertex("D");

    cycleGraph.addEdge("A", "B", 1);
    cycleGraph.addEdge("B", "C", -3);
    cycleGraph.addEdge("C", "B", 1);
    cycleGraph.addEdge("C", "D", 2);

    RouteResult negativeCycle =
        directedBellmanFord(cycleGraph, "A", "D");

    check(
        negativeCycle.validCities &&
        negativeCycle.negativeCycle &&
        !negativeCycle.found,
        "Detects a negative cycle affecting destination"
    );

    // Test 31: Negative cycle cannot affect destination.
    DirectedGraph separateCycle;

    separateCycle.addVertex("A");
    separateCycle.addVertex("B");
    separateCycle.addVertex("C");
    separateCycle.addVertex("D");

    separateCycle.addEdge("A", "B", 2);
    separateCycle.addEdge("B", "C", -4);
    separateCycle.addEdge("C", "B", 1);
    separateCycle.addEdge("A", "D", 7);

    RouteResult unaffected =
        directedBellmanFord(separateCycle, "A", "D");

    check(
        unaffected.found &&
        !unaffected.negativeCycle &&
        unaffected.distance == 7,
        "Unrelated negative cycle preserves valid route"
    );

    // Test 32: Directed edges are one-way.
    RouteResult reverse =
        directedBellmanFord(directed, "C", "A");

    check(
        reverse.validCities &&
        !reverse.found &&
        !reverse.negativeCycle,
        "Directed edges do not imply reverse connections"
    );

    // Test 33: Invalid vertex is rejected.
    check(
        throwsInvalidArgument([&]() {
            directed.addEdge("A", "Unknown", 5);
        }),
        "DirectedGraph rejects unknown vertices"
    );

    // Helper to create a temporary CSV file for each test.
    auto writeTestCsv = [](
        const std::string& filename,
        const std::string& contents
    ) {
        std::ofstream file(filename);

        if (!file) {
            throw std::runtime_error(
                "Cannot create temporary test CSV."
            );
        }

        file << contents;
        file.close();

        if (!file) {
            throw std::runtime_error(
                "Failed to write temporary test CSV."
            );
        }
    };

    const std::string invalidCsvFile =
        "invalid_network_test.csv";

    // Create a graph that must survive failed imports.
    Graph protectedGraph;

    protectedGraph.addCity("SafeA", 0, 0);
    protectedGraph.addCity("SafeB", 1, 0);
    protectedGraph.addRoad("SafeA", "SafeB", 5);

    // Test 44: Reject an incorrect CSV header.
    writeTestCsv(
        invalidCsvFile,
        "wrong,header\n"
        "CITY,A,0,0\n"
    );

    bool badHeaderRejected = false;

    try {
        loadNetwork(protectedGraph, invalidCsvFile);
    }
    catch (const std::exception&) {
        badHeaderRejected = true;
    }

    check(
        badHeaderRejected,
        "Rejects incorrect CSV header"
    );


    // ===== TESTS 34-38: CSV PERSISTENCE =====

    // Test 34: Save and restore a network.
    Graph savedGraph;

    savedGraph.addCity("A", 0, 0);
    savedGraph.addCity("B", 1, 0);
    savedGraph.addCity("C", 2, 0);

    savedGraph.addRoad("A", "B", 3);
    savedGraph.addRoad("B", "C", 4);

    const string csvFile = "route_network_test.csv";

    saveNetwork(savedGraph, csvFile);

    Graph restoredGraph;
    loadNetwork(restoredGraph, csvFile);

    RouteResult restoredRoute =
        restoredGraph.calculateRoute("A", "C", false);

    check(
        restoredRoute.found &&
        restoredRoute.distance == 7 &&
        restoredRoute.path ==
            vector<string>({"A", "B", "C"}),
        "CSV save/load preserves shortest routes"
    );

    // Test 35: Coordinates survive serialization.
    City restoredB = restoredGraph.getCity("B");

    check(
        restoredB.x == 1.0 &&
        restoredB.y == 0.0,
        "CSV preserves city coordinates"
    );

    // Test 36: Loading replaces the existing graph.
    restoredGraph.addCity("Extra", 10, 10);
    loadNetwork(restoredGraph, csvFile);

    check(
        !restoredGraph.hasCity("Extra") &&
        restoredGraph.hasCity("A"),
        "CSV loading replaces the existing network"
    );

    // Test 37: Missing file does not destroy the graph.
    bool missingFileRejected = false;

    try {
        loadNetwork(
            restoredGraph,
            "file_that_does_not_exist_123.csv"
        );
    }
    catch (const exception&) {
        missingFileRejected = true;
    }

    check(
        missingFileRejected &&
        restoredGraph.hasCity("A"),
        "Missing CSV file preserves existing network"
    );

    // Test 38: Parallel roads remain intact.
    Graph parallelSave;

    parallelSave.addCity("X", 0, 0);
    parallelSave.addCity("Y", 0, 0);

    parallelSave.addRoad("X", "Y", 3);
    parallelSave.addRoad("X", "Y", 8);

    saveNetwork(parallelSave, csvFile);

    Graph parallelLoad;
    loadNetwork(parallelLoad, csvFile);

    check(
        parallelLoad.getNeighbors("X").size() == 2,
        "CSV preserves parallel roads"
    );

    // Remove the temporary test file.
    std::remove(csvFile.c_str());


    // ===== TESTS 39-43: ROAD MANAGEMENT =====

    // Test 39: Update an existing road.
    Graph editable;

    editable.addCity("A", 0, 0);
    editable.addCity("B", 1, 0);
    editable.addCity("C", 2, 0);

    editable.addRoad("A", "B", 10);
    editable.addRoad("B", "C", 5);

    bool updated = editable.updateRoad("A", "B", 10, 3);

    RouteResult afterUpdate =
        editable.calculateRoute("A", "C", false);

    check(
        updated &&
        afterUpdate.found &&
        afterUpdate.distance == 8,
        "Updating a road changes shortest-path cost"
    );

    // Test 40: Delete an existing road.
    bool removed = editable.removeRoad("B", "C", 5);

    RouteResult afterRemoval =
        editable.calculateRoute("A", "C", false);

    check(
        removed &&
        !afterRemoval.found,
        "Removing a road changes connectivity"
    );

    // Test 41: Reject an invalid updated distance.
    check(
        throwsInvalidArgument([&]() {
            editable.updateRoad("A", "B", 3, -5);
        }),
        "Updating a road rejects negative distance"
    );

    // Test 42: Return false for a nonexistent road.
    check(
        !editable.removeRoad("A", "C", 7) &&
        !editable.updateRoad("A", "C", 7, 9),
        "Road operations handle nonexistent connections"
    );

    // Test 43: Preserve parallel roads.
    Graph parallelEdit;

    parallelEdit.addCity("X", 0, 0);
    parallelEdit.addCity("Y", 0, 0);

    parallelEdit.addRoad("X", "Y", 5);
    parallelEdit.addRoad("X", "Y", 10);

    bool parallelRemoved =
        parallelEdit.removeRoad("X", "Y", 10);

    RouteResult parallelRoute =
        parallelEdit.calculateRoute("X", "Y", false);

    check(
        parallelRemoved &&
        parallelRoute.found &&
        parallelRoute.distance == 5 &&
        parallelEdit.getNeighbors("X").size() == 1 &&
        parallelEdit.getNeighbors("Y").size() == 1,
        "Removing one road preserves parallel connections"
    );



    // Test 44: Reject an incorrect CSV header.
    writeTestCsv(
        invalidCsvFile,
        "wrong,header\n"
        "CITY,A,0,0\n"
    );

    // Test 45: Reject a road referencing an unknown city.
    writeTestCsv(
        invalidCsvFile,
        "type,field1,field2,field3\n"
        "CITY,A,0,0\n"
        "ROAD,A,Unknown,5\n"
    );

    bool unknownRoadRejected = false;
    bool roadErrorHasLine = false;

    try {
        loadNetwork(protectedGraph, invalidCsvFile);
    }
    catch (const std::exception& e) {
        unknownRoadRejected = true;

        roadErrorHasLine =
            std::string(e.what()).find("CSV line 3") !=
            std::string::npos;
    }

    check(
        unknownRoadRejected && roadErrorHasLine,
        "Rejects unknown road endpoint with line number"
    );

    // Test 46: Reject non-finite coordinates.
    writeTestCsv(
        invalidCsvFile,
        "type,field1,field2,field3\n"
        "CITY,A,nan,0\n"
    );

    bool nanRejected = false;

    try {
        loadNetwork(protectedGraph, invalidCsvFile);
    }
    catch (const std::exception&) {
        nanRejected = true;
    }

    check(
        nanRejected,
        "Rejects NaN coordinates"
    );

    // Test 47: Reject a negative road distance.
    writeTestCsv(
        invalidCsvFile,
        "type,field1,field2,field3\n"
        "CITY,A,0,0\n"
        "CITY,B,1,0\n"
        "ROAD,A,B,-5\n"
    );

    bool negativeRoadRejected = false;

    try {
        loadNetwork(protectedGraph, invalidCsvFile);
    }
    catch (const std::exception&) {
        negativeRoadRejected = true;
    }

    check(
        negativeRoadRejected,
        "Rejects negative road distance in CSV"
    );

    // Test 48: Failed imports preserve the original network.
    RouteResult preservedRoute =
        protectedGraph.calculateRoute(
            "SafeA", "SafeB", false
        );

    check(
        protectedGraph.hasCity("SafeA") &&
        protectedGraph.hasCity("SafeB") &&
        !protectedGraph.hasCity("A") &&
        preservedRoute.found &&
        preservedRoute.distance == 5,
        "Failed CSV imports preserve existing graph"
    );

    // Test 49: Reject a malformed quoted CSV field.
    writeTestCsv(
        invalidCsvFile,
        "type,field1,field2,field3\n"
        "CITY,\"Unclosed,0,0\n"
    );

    bool malformedQuoteRejected = false;

    try {
        loadNetwork(protectedGraph, invalidCsvFile);
    }
    catch (const std::exception&) {
        malformedQuoteRejected = true;
    }

    check(
        malformedQuoteRejected,
        "Rejects malformed CSV quotation"
    );

    // Delete the temporary test file.
    std::remove(invalidCsvFile.c_str());

    // Test 50: Reject NaN coordinates at Graph level.
    Graph coordinateGraph;

    check(
        throwsInvalidArgument([&]() {
            coordinateGraph.addCity(
                "InvalidNaN",
                std::numeric_limits<double>::quiet_NaN(),
                0
            );
        }),
        "Graph rejects NaN city coordinates"
    );

    // Test 51: Reject infinite coordinates.
    check(
        throwsInvalidArgument([&]() {
            coordinateGraph.addCity(
                "InvalidInfinity",
                0,
                std::numeric_limits<double>::infinity()
            );
        }),
        "Graph rejects infinite city coordinates"
    );

    // Use a separate graph to avoid changing earlier tests.
    Graph edgeCaseGraph;

    edgeCaseGraph.addCity("A", 0, 0);
    edgeCaseGraph.addCity("B", 1, 0);
    edgeCaseGraph.addCity("C", 2, 0);
    edgeCaseGraph.addCity("D", 10, 0);

    edgeCaseGraph.addRoad("A", "B", 4);
    edgeCaseGraph.addRoad("B", "C", 3);

    // D has no connections.

    // Test 52: Dijkstra handles disconnected cities.
    RouteResult disconnectedDijkstra =
        edgeCaseGraph.calculateRoute("A", "D", false);

    check(
        disconnectedDijkstra.validCities &&
        !disconnectedDijkstra.found &&
        !disconnectedDijkstra.negativeCycle,
        "Dijkstra handles disconnected cities"
    );

    // Test 53: A* handles disconnected cities.
    RouteResult disconnectedAStar =
        edgeCaseGraph.calculateRoute("A", "D", true);

    check(
        disconnectedAStar.validCities &&
        !disconnectedAStar.found &&
        !disconnectedAStar.negativeCycle,
        "A* handles disconnected cities"
    );

    // Test 54: Floyd-Warshall handles unreachable paths.
    AllPairsResult disconnectedMatrix =
        floydWarshall(edgeCaseGraph);

    RouteResult disconnectedFloyd =
        getFloydWarshallRoute(
            disconnectedMatrix, "A", "D"
        );

    check(
        disconnectedFloyd.validCities &&
        !disconnectedFloyd.found &&
        !disconnectedMatrix.negativeCycle,
        "Floyd-Warshall handles unreachable destinations"
    );

    // Test 55: Road update affects both directions.
    bool updatedBothDirections =
        edgeCaseGraph.updateRoad("A", "B", 4, 6);

    RouteResult forwardUpdated =
        edgeCaseGraph.calculateRoute("A", "B", false);

    RouteResult reverseUpdated =
        edgeCaseGraph.calculateRoute("B", "A", false);

    check(
        updatedBothDirections &&
        forwardUpdated.found &&
        reverseUpdated.found &&
        forwardUpdated.distance == 6 &&
        reverseUpdated.distance == 6,
        "Road update preserves bidirectional consistency"
    );

    // Test 56: Removing a road affects both directions.
    bool removedBothDirections =
        edgeCaseGraph.removeRoad("A", "B", 6);

    RouteResult forwardRemoved =
        edgeCaseGraph.calculateRoute("A", "B", false);

    RouteResult reverseRemoved =
        edgeCaseGraph.calculateRoute("B", "A", false);

    check(
        removedBothDirections &&
        !forwardRemoved.found &&
        !reverseRemoved.found,
        "Road deletion preserves bidirectional consistency"
    );

    // Test 57: Kruskal handles disconnected networks.
    // After removing A-B, the components are:
    // {A}, {B,C}, {D}.
    MSTResult disconnectedForest =
        kruskalMST(edgeCaseGraph);

    check(
        !disconnectedForest.connected &&
        disconnectedForest.components == 3 &&
        disconnectedForest.edges.size() == 1 &&
        disconnectedForest.totalWeight == 3,
        "Kruskal produces a minimum spanning forest"
    );

    // Test 58: Unreachable negative cycles do not
    // invalidate a valid directed shortest path.
    DirectedGraph isolatedNegativeCycle;

    isolatedNegativeCycle.addVertex("S");
    isolatedNegativeCycle.addVertex("T");
    isolatedNegativeCycle.addVertex("X");
    isolatedNegativeCycle.addVertex("Y");

    isolatedNegativeCycle.addEdge("S", "T", 5);
    isolatedNegativeCycle.addEdge("X", "Y", -4);
    isolatedNegativeCycle.addEdge("Y", "X", 1);

    RouteResult unaffectedDirected =
        directedBellmanFord(
            isolatedNegativeCycle, "S", "T"
        );

    check(
        unaffectedDirected.validCities &&
        unaffectedDirected.found &&
        !unaffectedDirected.negativeCycle &&
        unaffectedDirected.distance == 5,
        "Bellman-Ford ignores unreachable negative cycles"
    );

    // Test 59: Shortest path from a city to itself.
    RouteResult sameCity =
        edgeCaseGraph.calculateRoute("A", "A", false);

    check(
        sameCity.validCities &&
        sameCity.found &&
        sameCity.distance == 0 &&
        sameCity.path ==
            std::vector<std::string>({"A"}),
        "Dijkstra handles source equal to destination"
    );

    // Test 60: Floyd-Warshall handles a disconnected graph.
    Graph disconnectedSafety;

    disconnectedSafety.addCity("A", 0, 0);
    disconnectedSafety.addCity("B", 1, 0);
    disconnectedSafety.addCity("C", 10, 0);

    disconnectedSafety.addRoad("A", "B", 3);

    AllPairsResult safetyMatrix =
        floydWarshall(disconnectedSafety);

    RouteResult unreachableSafety =
        getFloydWarshallRoute(
            safetyMatrix, "A", "C"
        );

    check(
        unreachableSafety.validCities &&
        !unreachableSafety.found &&
        !safetyMatrix.negativeCycle,
        "Floyd-Warshall preserves unreachable distances"
    );

    // Test 61: Floyd-Warshall handles large edge weights.
    Graph largeWeightGraph;

    largeWeightGraph.addCity("A", 0, 0);
    largeWeightGraph.addCity("B", 1, 0);
    largeWeightGraph.addCity("C", 2, 0);

    largeWeightGraph.addRoad("A", "B", 2000000000);
    largeWeightGraph.addRoad("B", "C", 2000000000);

    AllPairsResult largeMatrix =
        floydWarshall(largeWeightGraph);

    RouteResult largeRoute =
        getFloydWarshallRoute(
            largeMatrix, "A", "C"
        );

    check(
        largeRoute.validCities &&
        largeRoute.found &&
        largeRoute.distance == 4000000000LL,
        "Floyd-Warshall handles distances above INT_MAX"
    );


    cout << "\n===== TEST SUMMARY =====\n";
    cout << "Passed: " << passed << '\n';
    cout << "Failed: " << failed << '\n';

    return failed == 0 ? 0 : 1;
}
