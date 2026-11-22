
#include "algorithms/Kruskal.h"

#include <algorithm>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using namespace std;

// Disjoint Set Union (Union-Find)
class DisjointSet {
private:
    vector<size_t> parent;
    vector<int> rank;

public:
    explicit DisjointSet(size_t n)
        : parent(n), rank(n, 0) {
        iota(parent.begin(), parent.end(), 0);
    }

    size_t find(size_t x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);

        return parent[x];
    }

    bool unite(size_t a, size_t b) {
        a = find(a);
        b = find(b);

        if (a == b)
            return false;

        if (rank[a] < rank[b])
            swap(a, b);

        parent[b] = a;

        if (rank[a] == rank[b])
            ++rank[a];

        return true;
    }
};

MSTResult kruskalMST(const Graph& graph) {
    MSTResult result;

    vector<string> cities = graph.getCityNames();
    sort(cities.begin(), cities.end());

    const size_t n = cities.size();
    result.components = n;

    if (n == 0) {
        result.connected = true;
        return result;
    }

    unordered_map<string, size_t> index;

    for (size_t i = 0; i < n; ++i)
        index[cities[i]] = i;

    vector<MSTEdge> allEdges;

    // Graph stores each undirected road twice.
    // Keep only one direction for each road.
    for (const string& city : cities) {
        for (const auto& [neighbor, weight] :
             graph.getNeighbors(city)) {

            if (city < neighbor) {
                allEdges.push_back({
                    city, neighbor, weight
                });
            }
        }
    }

    // Sort edges from cheapest to most expensive.
    sort(allEdges.begin(), allEdges.end(),
         [](const MSTEdge& a, const MSTEdge& b) {
             if (a.weight != b.weight)
                 return a.weight < b.weight;

             if (a.from != b.from)
                 return a.from < b.from;

             return a.to < b.to;
         });

    DisjointSet dsu(n);

    for (const MSTEdge& edge : allEdges) {
        size_t u = index.at(edge.from);
        size_t v = index.at(edge.to);

        // Only select edges that connect two groups.
        if (dsu.unite(u, v)) {
            if (result.totalWeight >
                numeric_limits<long long>::max() - edge.weight) {
                throw overflow_error("MST total weight overflow.");
            }

            result.edges.push_back(edge);
            result.totalWeight += edge.weight;
            --result.components;

            // A spanning tree has exactly n-1 edges.
            if (result.components == 1)
                break;
        }
    }

    result.connected = (result.components == 1);

    return result;
}
