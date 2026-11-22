
#include "algorithms/Dijkstra.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;

RouteResult dijkstraShortestPath(
    const Graph& graph,
    const string& start,
    const string& destination
) {
    RouteResult result;

    if (!graph.hasCity(start) || !graph.hasCity(destination))
        return result;

    result.validCities = true;

    const long long INF = numeric_limits<long long>::max();

    unordered_map<string, long long> distance;
    unordered_map<string, string> parent;

    using State = pair<long long, string>;

    priority_queue<
        State,
        vector<State>,
        greater<State>
    > pq;

    distance[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [currentDistance, current] = pq.top();
        pq.pop();

        if (currentDistance != distance[current])
            continue;

        ++result.explored;

        if (current == destination)
            break;

        for (const auto& [neighbor, weight] :
             graph.getNeighbors(current)) {

            if (currentDistance > INF - weight)
                continue;

            long long newDistance =
                currentDistance + weight;

            auto it = distance.find(neighbor);

            if (it == distance.end() ||
                newDistance < it->second) {

                distance[neighbor] = newDistance;
                parent[neighbor] = current;

                pq.push({newDistance, neighbor});
            }
        }
    }

    if (!distance.count(destination))
        return result;

    result.found = true;
    result.distance = distance.at(destination);

    string current = destination;

    while (current != start) {
        result.path.push_back(current);
        current = parent.at(current);
    }

    result.path.push_back(start);

    reverse(result.path.begin(), result.path.end());

    return result;
}
