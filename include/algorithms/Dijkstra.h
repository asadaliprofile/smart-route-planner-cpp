
#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Graph.h"

RouteResult dijkstraShortestPath(
    const Graph& graph,
    const std::string& start,
    const std::string& destination
);

#endif
