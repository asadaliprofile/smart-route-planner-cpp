
#include "algorithms/AStar.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;

RouteResult aStarShortestPath(
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

    // State: estimated total, actual distance, city.
    using State = tuple<double, long long, string>;

    priority_queue<
        State,
        vector<State>,
        greater<State>
    > pq;

    distance[start] = 0;

    pq.push({
        graph.straightLineDistance(start, destination),
        0,
        start
    });

    while (!pq.empty()) {
        auto [priority, currentDistance, current] = pq.top();
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

                double estimatedCost =
                    static_cast<double>(newDistance) +
                    graph.straightLineDistance(
                        neighbor, destination
                    );

                pq.push({
                    estimatedCost,
                    newDistance,
                    neighbor
                });
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
