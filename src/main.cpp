#include <iostream>
#include <string>
#include <limits>
#include <stdexcept>
#include <iomanip>
#include <exception>



#include "Graph.h"
#include "algorithms/BFS.h"
#include "algorithms/BellmanFord.h"
#include "algorithms/FloydWarshall.h"
#include "algorithms/Kruskal.h"
#include "DirectedGraphMenu.h"
#include "NetworkIO.h"
#include "InputUtils.h"

#include "InputUtils.h"

using namespace std;

int main() {
    Graph graph;
    DirectedGraph directedGraph;

    // Coordinates represent positions in kilometres.
    graph.addCity("A", 0, 0);
    graph.addCity("B", 0.5, 0);
    graph.addCity("C", 0, 0.5);
    graph.addCity("D", 0.5, 0.5);

    graph.addRoad("A", "B", 4);
    graph.addRoad("A", "C", 2);
    graph.addRoad("B", "D", 3);
    graph.addRoad("C", "D", 3);
    graph.addRoad("B", "C", 1);

    int choice;

    while (true) {
        cout << "\n===== SMART ROUTE PLANNER =====\n";
        cout << "0. Exit\n";
        cout << "1. Display Road Network\n";
        cout << "2. Find Route (Dijkstra)\n";
        cout << "3. Find Route (A*)\n";
        cout << "4. Add City\n";
        cout << "5. Add Road\n";
        cout << "6. Find Route (BFS - Fewest Roads)\n";
        cout << "7. Find Route (Bellman-Ford)\n";
        cout << "8. All-Pairs Shortest Paths (Floyd-Warshall)\n";
        cout << "9. Minimum Spanning Tree (Kruskal)\n";
        cout << "10. Directed Graph Mode\n";
        cout << "11. Save Road Network to CSV\n";
        cout << "12. Load Road Network from CSV\n";
        cout << "13. Update Existing Road\n";
        cout << "14. Delete Existing Road\n";
                
        if (!readInteger(
            cin,
            cout,
            "Enter your choice: ",
            choice
            )) {
            cout << "\nInput closed. Exiting...\n";
            break;
        }

        try {
            if (choice == 0) {
                cout << "Exiting Smart Route Planner...\n";
                break;
            }
            if (choice == 1) {
                graph.displayGraph();
            }
            else if (choice == 2){                
                string start, destination;

                if (!readText(
                        cin, cout, "Starting city: ", start))
                    break;

                if (!readText(
                        cin, cout, "Destination city: ", destination))
                    break;
                graph.findRoute(start, destination, choice==2);
            } 
            else if (choice == 3) {                
                string start, destination;

                if (!readText(
                        cin, cout, "Starting city: ", start))
                    break;

                if (!readText(
                        cin, cout, "Destination city: ", destination))
                    break;

                graph.findRoute(start, destination, choice==3);
            }
            
            else if (choice == 4) {
                string name;
                double x, y;

                if (!readText(
                        cin, cout, "City name: ", name))
                    break;

                if (!readDouble(
                        cin, cout, "X coordinate: ", x))
                    break;

                if (!readDouble(
                        cin, cout, "Y coordinate: ", y))
                    break;

                try {
                    graph.addCity(name, x, y);
                    cout << "City added successfully!\n";
                }
                catch (const exception& e) {
                    cout << "Error: " << e.what() << '\n';
                }
            }
            
            else if (choice == 5) {
                string city1, city2;
                int distance;

                if (!readText(
                        cin, cout, "First city: ", city1))
                    break;

                if (!readText(
                        cin, cout, "Second city: ", city2))
                    break;

                if (!readInteger(
                        cin, cout, "Road distance: ", distance))
                    break;

                try {
                    graph.addRoad(city1, city2, distance);
                    cout << "Road added successfully!\n";
                }
                catch (const exception& e) {
                    cout << "Error: " << e.what() << '\n';
                }
            }


            else if (choice == 6) {
                string start, destination;

                if (!readText(
                        cin, cout, "Starting city: ", start))
                    break;

                if (!readText(
                        cin, cout, "Destination city: ", destination))
                    break;

                RouteResult result =
                    breadthFirstSearch(graph, start, destination);

                cout << "\nAlgorithm: BFS\n";

                if (!result.validCities) {
                    cout << "City not found!\n";
                }
                else if (!result.found) {
                    cout << "No route found!\n";
                }
                else {
                    cout << "Route: ";

                    for (size_t i = 0; i < result.path.size(); ++i) {
                        if (i > 0)
                            cout << " -> ";

                        cout << result.path[i];
                    }

                    cout << "\nRoads used: "
                        << result.path.size() - 1;

                    cout << "\nRoute distance: "
                        << result.distance << " km\n";
                }

                cout << "Cities explored: "
                    << result.explored << '\n';
            }


            else if (choice == 7) {                
                string start, destination;

                if (!readText(
                        cin, cout, "Starting city: ", start))
                    break;

                if (!readText(
                        cin, cout, "Destination city: ", destination))
                    break;


                RouteResult result =
                    bellmanFordShortestPath(
                        graph, start, destination
                    );

                cout << "\nAlgorithm: Bellman-Ford\n";

                if (!result.validCities) {
                    cout << "City not found!\n";
                }
                else if (result.negativeCycle) {
                    cout << "Reachable negative-weight cycle detected!\n";
                }
                else if (!result.found) {
                    cout << "No route found!\n";
                }
                else {
                    cout << "Shortest Route: ";

                    for (size_t i = 0;
                        i < result.path.size(); ++i) {

                        if (i > 0)
                            cout << " -> ";

                        cout << result.path[i];
                    }

                    cout << "\nTotal Distance: "
                        << result.distance << " km\n";
                }

                cout << "Relaxation passes: "
                    << result.explored << '\n';
            }


            else if (choice == 8) {
                vector<string> names = graph.getCityNames();

                // Avoid enormous output and expensive computation.
                if (names.size() > 20) {
                    cout << "This display supports up to 20 cities.\n";
                    continue;
                }

                AllPairsResult matrix = floydWarshall(graph);

                cout << "\n===== ALL-PAIRS DISTANCE MATRIX =====\n";

                cout << setw(12) << "From/To";

                for (const string& city : matrix.cities)
                    cout << setw(12) << city;

                cout << '\n';

                for (size_t i = 0; i < matrix.cities.size(); ++i) {
                    cout << setw(12) << matrix.cities[i];

                    for (size_t j = 0; j < matrix.cities.size(); ++j) {
                        if (matrix.distance[i][j] ==
                            AllPairsResult::INF) {
                            cout << setw(12) << "INF";
                        }
                        else {
                            cout << setw(12)
                                << matrix.distance[i][j];
                        }
                    }

                    cout << '\n';
                }

                if (matrix.negativeCycle) {
                    cout << "\nWarning: Negative cycle detected.\n";
                    cout << "Some shortest distances are undefined.\n";
                }
                else {
                    string start, destination;

                    cout << "\nStarting city: ";
                    if (!(cin >> start)) break;

                    cout << "Destination city: ";
                    if (!(cin >> destination)) break;

                    RouteResult route =
                        getFloydWarshallRoute(matrix, start, destination);

                    if (!route.validCities) {
                        cout << "City not found!\n";
                    }
                    else if (!route.found) {
                        cout << "No route found!\n";
                    }
                    else {
                        cout << "Shortest Route: ";

                        for (size_t i = 0; i < route.path.size(); ++i) {
                            if (i > 0)
                                cout << " -> ";

                            cout << route.path[i];
                        }

                        cout << "\nTotal Distance: "
                            << route.distance << " km\n";
                    }
                }
            }

            else if (choice == 9) {
                MSTResult mst = kruskalMST(graph);

                cout << "\n===== KRUSKAL'S ALGORITHM =====\n";

                if (mst.components == 0) {
                    cout << "The road network is empty.\n";
                }
                else {
                    if (mst.connected) {
                        cout << "Minimum Spanning Tree:\n";
                    }
                    else {
                        cout << "Minimum Spanning Forest:\n";
                        cout << "The network is disconnected.\n";
                    }

                    for (const MSTEdge& edge : mst.edges) {
                        cout << edge.from << " -- "
                            << edge.to << " ("
                            << edge.weight << " km)\n";
                    }

                    cout << "\nTotal Network Weight: "
                        << mst.totalWeight << " km\n";

                    cout << "Selected Roads: "
                        << mst.edges.size() << '\n';

                    cout << "Connected Components: "
                        << mst.components << '\n';
                }
            }

            else if (choice == 10) {
                runDirectedGraphMenu(directedGraph);
            }
            
            else if (choice == 11) {
                string filename;

                if (!readText(
                        cin, cout,
                        "Filename to save (e.g. network.csv): ",
                        filename))
                    break;

                string path = "data/" + filename;

                try {
                    saveNetwork(graph, path);
                    cout << "Network saved to: "
                        << path << '\n';
                }
                catch (const exception& e) {
                    cout << "Save error: "
                        << e.what() << '\n';
                }
            }
            
            else if (choice == 12) {
                string filename;

                if (!readText(
                        cin, cout,
                        "Filename to load (e.g. network.csv): ",
                        filename))
                    break;

                string path = "data/" + filename;

                try {
                    loadNetwork(graph, path);
                    cout << "Network loaded from: "
                        << path << '\n';
                }
                catch (const exception& e) {
                    cout << "Load error: "
                        << e.what() << '\n';
                }
            }

            else if (choice == 13) {
                string city1, city2;
                int oldDistance, newDistance;

                if (!readText(cin, cout, "First city: ", city1))
                    break;

                if (!readText(cin, cout, "Second city: ", city2))
                    break;

                if (!readInteger(
                        cin, cout,
                        "Current road distance: ",
                        oldDistance))
                    break;

                if (!readInteger(
                        cin, cout,
                        "New road distance: ",
                        newDistance))
                    break;

                try {
                    bool updated = graph.updateRoad(
                        city1, city2, oldDistance, newDistance
                    );

                    if (updated)
                        cout << "Road updated successfully!\n";
                    else
                        cout << "Matching road not found.\n";
                }
                catch (const exception& e) {
                    cout << "Error: " << e.what() << '\n';
                }
            }

            else if (choice == 14) {
                string city1, city2;
                int distance;

                if (!readText(
                        cin, cout, "First city: ", city1))
                    break;

                if (!readText(
                        cin, cout, "Second city: ", city2))
                    break;

                if (!readInteger(
                        cin, cout,
                        "Road distance to delete: ",
                        distance))
                    break;

                try {
                    bool removed = graph.removeRoad(
                        city1,
                        city2,
                        distance
                    );

                    if (removed) {
                        cout << "Road deleted successfully!\n";
                    }
                    else {
                        cout << "Matching road not found.\n";
                    }
                }
                catch (const exception& e) {
                    cout << "Error: " << e.what() << '\n';
                }
            }

            else {
                cout << "Invalid choice!\n";
            }
        }
        catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << '\n';

            if (cin.fail() && !cin.eof()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
    }

    return 0;
}
