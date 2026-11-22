
#ifndef NETWORK_IO_H
#define NETWORK_IO_H

#include "Graph.h"

#include <string>

void saveNetwork(
    const Graph& graph,
    const std::string& filename
);

void loadNetwork(
    Graph& graph,
    const std::string& filename
);

#endif
