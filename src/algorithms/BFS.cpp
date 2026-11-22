
#include "algorithms/BFS.h"

#include <algorithm>
#include <limits>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

RouteResult breadthFirstSearch(
    const Graph& graph,
    const string& start,
    const string& destination
) {
    RouteResult result;

    // Check whether both cities exist.
    if (!graph.hasCity(start) ||
        !graph.hasCity(destination)) {
        return result;
    }

    result.validCities = true;

    queue<string> q;
    unordered_set<string> visited;

    unordered_map<string, string> parent;
    unordered_map<string, long long> distance;

    q.push(start);
    visited.insert(start);
    distance[start] = 0;

    while (!q.empty()) {
        string current = q.front();
        q.pop();

        ++result.explored;

        if (current == destination) {
            result.found = true;
            result.distance = distance.at(current);
            break;
        }

        // Visit every undiscovered neighbor.
        for (const auto& [neighbor, weight] :
             graph.getNeighbors(current)) {

            if (visited.count(neighbor))
                continue;

            long long currentDistance = distance.at(current);

            // Guard against integer overflow.
            if (currentDistance >
                numeric_limits<long long>::max() - weight) {
                continue;
            }

            visited.insert(neighbor);
            parent[neighbor] = current;

            distance[neighbor] = currentDistance + weight;

            q.push(neighbor);
        }
    }

    if (!result.found)
        return result;

    // Reconstruct the path.
    string current = destination;

    while (current != start) {
        result.path.push_back(current);
        current = parent.at(current);
    }

    result.path.push_back(start);

    reverse(result.path.begin(), result.path.end());

    return result;
}
