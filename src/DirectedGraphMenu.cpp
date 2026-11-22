
#include "DirectedGraphMenu.h"
#include "algorithms/DirectedBellmanFord.h"
#include "InputUtils.h"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace {

void displayDirectedGraph(const DirectedGraph& graph) {
    cout << "\n===== DIRECTED GRAPH =====\n";

    const auto& vertices = graph.getVertices();
    const auto& edges = graph.getEdges();

    cout << "Vertices: " << vertices.size() << '\n';
    cout << "Directed edges: " << edges.size() << '\n';

    if (vertices.empty()) {
        cout << "Graph is empty.\n";
        return;
    }

    for (size_t i = 0; i < vertices.size(); ++i) {
        cout << vertices[i] << " -> ";

        bool hasOutgoingEdge = false;

        for (const auto& edge : edges) {
            if (edge.from == i) {
                cout << vertices.at(edge.to)
                     << "(" << edge.weight << ") ";

                hasOutgoingEdge = true;
            }
        }

        if (!hasOutgoingEdge)
            cout << "(no outgoing edges)";

        cout << '\n';
    }
}

void printRouteResult(const RouteResult& result) {
    if (!result.validCities) {
        cout << "Vertex not found!\n";
        return;
    }

    if (result.negativeCycle) {
        cout << "A negative-weight cycle affects "
             << "this destination.\n";
        cout << "No finite shortest-path cost exists.\n";
        return;
    }

    if (!result.found) {
        cout << "No route found!\n";
        return;
    }

    cout << "Shortest Route: ";

    for (size_t i = 0; i < result.path.size(); ++i) {
        if (i > 0)
            cout << " -> ";

        cout << result.path[i];
    }

    cout << "\nTotal Cost: "
         << result.distance << '\n';

    cout << "Relaxation passes: "
         << result.explored << '\n';
}

void loadExampleGraph(DirectedGraph& graph) {
    // Use temporary graph to avoid a partially loaded demo.
    DirectedGraph example;

    example.addVertex("A");
    example.addVertex("B");
    example.addVertex("C");
    example.addVertex("D");

    example.addEdge("A", "B", 4);
    example.addEdge("A", "C", 10);
    example.addEdge("B", "C", -2);
    example.addEdge("C", "D", 3);

    graph = example;

    cout << "Example graph loaded successfully.\n";
    cout << "Try finding a route from A to D.\n";
}

} // namespace

void runDirectedGraphMenu(DirectedGraph& graph) {
    int choice = -1;

    while (true) {
        cout << "\n===== DIRECTED GRAPH MODE =====\n";
        cout << "1. Add Vertex\n";
        cout << "2. Add Directed Edge\n";
        cout << "3. Display Directed Graph\n";
        cout << "4. Find Route (Bellman-Ford)\n";
        cout << "5. Load Example Graph\n";
        cout << "0. Return to Main Menu\n";
        
        if (!readInteger(
        cin, cout, "Enter choice: ", choice))
            return;

        if (choice == 0) {
            cout << "Returning to main menu...\n";
            return;
        }

        try {
            if (choice == 1) {
                string name;

            if (!readText(cin, cout, "Vertex name: ", name))
                return;

            graph.addVertex(name);
            cout << "Vertex added successfully!\n";
            }
            else if (choice == 2) {
                
                string from, to;
                int weight;

                if (!readText(
                        cin, cout, "Starting vertex: ", from))
                    return;

                if (!readText(
                        cin, cout, "Destination vertex: ", to))
                    return;

                if (!readInteger(
                        cin, cout,
                        "Edge weight (negative allowed): ",
                        weight))
                    return;

                graph.addEdge(from, to, weight);
                cout << "Directed edge added successfully!\n";
            }
            else if (choice == 3) {
                displayDirectedGraph(graph);
            }
            else if (choice == 4) {
                string start, destination;

                if (!readText(
                        cin, cout, "Starting vertex: ", start))
                    return;

                if (!readText(
                        cin, cout, "Destination vertex: ", destination))
                    return;

                RouteResult result = directedBellmanFord(
                    graph, start, destination
                );

                cout << "\n===== BELLMAN-FORD RESULT =====\n";
                printRouteResult(result);
            }
            else if (choice == 5) {
                loadExampleGraph(graph);
            }
            else {
                cout << "Invalid choice. Try again.\n";
            }
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << '\n';
        }
    }
}
