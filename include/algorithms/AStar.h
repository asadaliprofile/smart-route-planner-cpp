
#ifndef ASTAR_H
#define ASTAR_H

#include "Graph.h"

RouteResult aStarShortestPath(
    const Graph& graph,
    const std::string& start,
    const std::string& destination
);

#endif
