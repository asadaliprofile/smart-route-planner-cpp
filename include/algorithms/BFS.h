#ifndef BFS_H
#define BFS_H

#include "Graph.h"

#include <string>

RouteResult breadthFirstSearch(
    const Graph& graph,
    const std::string& start,
    const std::string& destination
);

#endif
