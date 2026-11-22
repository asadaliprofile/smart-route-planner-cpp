
#ifndef KRUSKAL_H
#define KRUSKAL_H

#include "Graph.h"

#include <cstddef>
#include <string>
#include <vector>

struct MSTEdge {
    std::string from;
    std::string to;
    int weight;
};

struct MSTResult {
    std::vector<MSTEdge> edges;
    long long totalWeight = 0;
    std::size_t components = 0;
    bool connected = false;
};

MSTResult kruskalMST(const Graph& graph);

#endif
