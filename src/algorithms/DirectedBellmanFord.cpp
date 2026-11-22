#include "algorithms/DirectedBellmanFord.h"

#include <algorithm>
#include <limits>
#include <queue>
#include <vector>

using namespace std;

RouteResult directedBellmanFord(
    const DirectedGraph& graph,
    const string& start,
    const string& destination
) {
    RouteResult result;

    if (!graph.hasVertex(start) ||
        !graph.hasVertex(destination))
        return result;

    result.validCities = true;

    const auto& vertices = graph.getVertices();
    const auto& edges = graph.getEdges();

    const size_t n = vertices.size();
    const size_t source = graph.indexOf(start);
    const size_t target = graph.indexOf(destination);

    const long long INF =
        numeric_limits<long long>::max();

    const long long MIN =
        numeric_limits<long long>::min();

    vector<long long> dist(n, INF);
    vector<int> parent(n, -1);

    dist[source] = 0;

    // Relax edges at most V-1 times.
    for (size_t pass = 1; pass < n; ++pass) {
        bool changed = false;

        for (const auto& edge : edges) {
            if (dist[edge.from] == INF)
                continue;

            long long d = dist[edge.from];

            if (edge.weight > 0 &&
                d > INF - edge.weight)
                continue;

            if (edge.weight < 0 &&
                d < MIN - edge.weight)
                continue;

            long long candidate = d + edge.weight;

            if (candidate < dist[edge.to]) {
                dist[edge.to] = candidate;
                parent[edge.to] =
                    static_cast<int>(edge.from);
                changed = true;
            }
        }

        ++result.explored; // Relaxation passes

        if (!changed)
            break;
    }

    // Identify vertices whose distances can still improve.
    vector<bool> affected(n, false);
    queue<size_t> q;

    for (const auto& edge : edges) {
        if (dist[edge.from] == INF)
            continue;

        long long d = dist[edge.from];

        if (edge.weight > 0 &&
            d > INF - edge.weight)
            continue;

        if (edge.weight < 0 &&
            d < MIN - edge.weight)
            continue;

        if (d + edge.weight < dist[edge.to]) {
            if (!affected[edge.to]) {
                affected[edge.to] = true;
                q.push(edge.to);
            }
        }
    }

    // Propagate negative-cycle effects along directed edges.
    vector<vector<size_t>> adjacency(n);

    for (const auto& edge : edges)
        adjacency[edge.from].push_back(edge.to);

    while (!q.empty()) {
        size_t current = q.front();
        q.pop();

        for (size_t neighbor : adjacency[current]) {
            if (!affected[neighbor]) {
                affected[neighbor] = true;
                q.push(neighbor);
            }
        }
    }

    // A reachable negative cycle makes this destination's
    // shortest-path cost undefined if it can reach the target.
    if (affected[target]) {
        result.negativeCycle = true;
        return result;
    }

    if (dist[target] == INF)
        return result;

    result.found = true;
    result.distance = dist[target];

    // Reconstruct the path.
    size_t current = target;

    while (current != source) {
        result.path.push_back(vertices[current]);

        if (parent[current] < 0 ||
            result.path.size() > n) {
            result.found = false;
            result.path.clear();
            return result;
        }

        current = static_cast<size_t>(parent[current]);
    }

    result.path.push_back(vertices[source]);

    reverse(result.path.begin(), result.path.end());

    return result;
}
