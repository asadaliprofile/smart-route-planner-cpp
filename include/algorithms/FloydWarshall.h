
#ifndef FLOYD_WARSHALL_H
#define FLOYD_WARSHALL_H

#include "Graph.h"

#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

struct AllPairsResult {
    static constexpr long long INF =
        std::numeric_limits<long long>::max() / 4;

    std::vector<std::string> cities;

    std::unordered_map<std::string, std::size_t> index;

    std::vector<std::vector<long long>> distance;

    // next[i][j] stores the next city on the path i -> j.
    std::vector<std::vector<int>> next;

    bool negativeCycle = false;
};

// Calculate shortest distances between all city pairs.
AllPairsResult floydWarshall(const Graph& graph);

// Retrieve one route from an already calculated matrix.
RouteResult getFloydWarshallRoute(
    const AllPairsResult& result,
    const std::string& start,
    const std::string& destination
);

#endif
