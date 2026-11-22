
#include "algorithms/FloydWarshall.h"

#include <algorithm>
#include <stdexcept>

using namespace std;

AllPairsResult floydWarshall(const Graph& graph) {
    AllPairsResult result;

    result.cities = graph.getCityNames();

    // Sort cities for consistent matrix ordering.
    sort(result.cities.begin(), result.cities.end());

    size_t n = result.cities.size();

    result.distance.assign(
        n, vector<long long>(n, AllPairsResult::INF)
    );

    result.next.assign(n, vector<int>(n, -1));

    // Map city names to matrix indices.
    for (size_t i = 0; i < n; ++i) {
        result.index[result.cities[i]] = i;

        result.distance[i][i] = 0;
        result.next[i][i] = static_cast<int>(i);
    }

    // Initialize direct road distances.
    for (size_t i = 0; i < n; ++i) {
        const string& city = result.cities[i];

        for (const auto& [neighbor, weight] :
             graph.getNeighbors(city)) {

            size_t j = result.index.at(neighbor);

            if (weight < result.distance[i][j]) {
                result.distance[i][j] = weight;
                result.next[i][j] = static_cast<int>(j);
            }
        }
    }

    // Floyd-Warshall dynamic programming.
    
    for (size_t k = 0; k < n; ++k) {
        for (size_t i = 0; i < n; ++i) {

            if (result.distance[i][k] == AllPairsResult::INF)
                continue;

            for (size_t j = 0; j < n; ++j) {

                if (result.distance[k][j] == AllPairsResult::INF)
                    continue;

                long long first = result.distance[i][k];
                long long second = result.distance[k][j];

                // INF is also our upper bound for
                // representable finite path costs.
                if (first > AllPairsResult::INF - second)
                    continue;

                long long candidate = first + second;

                if (candidate < result.distance[i][j]) {
                    result.distance[i][j] = candidate;
                    result.next[i][j] = result.next[i][k];
                }
            }
        }
    }


    // Detect any negative-weight cycle.
    for (size_t i = 0; i < n; ++i) {
        if (result.distance[i][i] < 0) {
            result.negativeCycle = true;
            break;
        }
    }

    return result;
}

RouteResult getFloydWarshallRoute(
    const AllPairsResult& matrix,
    const string& start,
    const string& destination
) {
    RouteResult route;

    auto from = matrix.index.find(start);
    auto to = matrix.index.find(destination);

    if (from == matrix.index.end() ||
        to == matrix.index.end()) {
        return route;
    }

    route.validCities = true;

    size_t i = from->second;
    size_t j = to->second;

    if (matrix.distance[i][j] == AllPairsResult::INF)
        return route;

    // Check whether a negative cycle affects this route.
    for (size_t k = 0; k < matrix.cities.size(); ++k) {
        if (matrix.distance[i][k] != AllPairsResult::INF &&
            matrix.distance[k][k] < 0 &&
            matrix.distance[k][j] != AllPairsResult::INF) {
            route.negativeCycle = true;
            return route;
        }
    }

    route.found = true;
    route.distance = matrix.distance[i][j];

    size_t current = i;
    route.path.push_back(matrix.cities[current]);

    while (current != j) {
        int nextIndex = matrix.next[current][j];

        if (nextIndex < 0)
            throw logic_error("Invalid path matrix.");

        current = static_cast<size_t>(nextIndex);
        route.path.push_back(matrix.cities[current]);

        // Defensive check against corrupted path data.
        if (route.path.size() > matrix.cities.size() + 1)
            throw logic_error("Path reconstruction cycle.");
    }

    return route;
}
