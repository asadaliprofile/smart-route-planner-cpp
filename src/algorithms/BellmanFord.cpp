
#include "algorithms/BellmanFord.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct BFEdge {
    string from;
    string to;
    int weight;
};

RouteResult bellmanFordShortestPath(
    const Graph& graph,
    const string& start,
    const string& destination
) {
    RouteResult result;

    if (!graph.hasCity(start) ||
        !graph.hasCity(destination)) {
        return result;
    }

    result.validCities = true;

    const long long INF =
        numeric_limits<long long>::max();

    const long long MIN =
        numeric_limits<long long>::min();

    vector<string> vertices = graph.getCityNames();

    // Collect all directed edge entries.
    // Each bidirectional road currently appears twice.
    vector<BFEdge> edges;

    for (const string& city : vertices) {
        for (const auto& [neighbor, weight] :
             graph.getNeighbors(city)) {

            edges.push_back({city, neighbor, weight});
        }
    }

    unordered_map<string, long long> dist;
    unordered_map<string, string> parent;

    for (const string& city : vertices) {
        dist[city] = INF;
    }

    dist[start] = 0;

    // Relax every edge up to V - 1 times.
    for (size_t i = 1; i < vertices.size(); ++i) {
        bool updated = false;

        for (const BFEdge& edge : edges) {
            if (dist[edge.from] == INF)
                continue;

            long long current = dist[edge.from];

            // Avoid signed integer overflow.
            if (edge.weight > 0 &&
                current > INF - edge.weight)
                continue;

            if (edge.weight < 0 &&
                current < MIN - edge.weight)
                continue;

            long long candidate =
                current + edge.weight;

            if (candidate < dist[edge.to]) {
                dist[edge.to] = candidate;
                parent[edge.to] = edge.from;
                updated = true;
            }
        }

        ++result.explored;

        // Stop if no distances changed.
        if (!updated)
            break;
    }

    // Check for reachable negative-weight cycles.
    for (const BFEdge& edge : edges) {
        if (dist[edge.from] == INF)
            continue;

        long long current = dist[edge.from];

        if (edge.weight > 0 &&
            current > INF - edge.weight)
            continue;

        if (edge.weight < 0 &&
            current < MIN - edge.weight)
            continue;

        if (current + edge.weight < dist[edge.to]) {
            result.negativeCycle = true;
            return result;
        }
    }

    if (dist[destination] == INF) {
        return result;
    }

    result.found = true;
    result.distance = dist[destination];

    // Reconstruct shortest route.
    string current = destination;

    while (current != start) {
        result.path.push_back(current);
        current = parent.at(current);
    }

    result.path.push_back(start);

    reverse(
        result.path.begin(),
        result.path.end()
    );

    return result;
}
