
#ifndef DIRECTED_BELLMAN_FORD_H
#define DIRECTED_BELLMAN_FORD_H

#include "DirectedGraph.h"
#include "Graph.h"

#include <string>

RouteResult directedBellmanFord(
    const DirectedGraph& graph,
    const std::string& start,
    const std::string& destination
);

#endif
