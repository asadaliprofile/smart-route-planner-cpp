
#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include "Graph.h"

#include <string>

RouteResult bellmanFordShortestPath(
    const Graph& graph,
    const std::string& start,
    const std::string& destination
);

#endif
