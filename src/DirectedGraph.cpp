
#include "DirectedGraph.h"

#include <stdexcept>

using namespace std;

void DirectedGraph::addVertex(const string& name) {
    if (name.empty())
        throw invalid_argument("Vertex name cannot be empty.");

    if (hasVertex(name))
        throw invalid_argument("Vertex already exists.");

    indices[name] = names.size();
    names.push_back(name);
}

void DirectedGraph::addEdge(
    const string& from,
    const string& to,
    int weight
) {
    if (!hasVertex(from) || !hasVertex(to))
        throw invalid_argument("Vertex does not exist.");

    edges.push_back({
        indexOf(from),
        indexOf(to),
        weight
    });
}

bool DirectedGraph::hasVertex(const string& name) const {
    return indices.count(name) > 0;
}

size_t DirectedGraph::indexOf(const string& name) const {
    return indices.at(name);
}

const vector<string>&
DirectedGraph::getVertices() const {
    return names;
}

const vector<DirectedEdge>&
DirectedGraph::getEdges() const {
    return edges;
}
