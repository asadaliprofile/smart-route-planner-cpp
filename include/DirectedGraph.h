#ifndef DIRECTED_GRAPH_H
#define DIRECTED_GRAPH_H

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

struct DirectedEdge {
    std::size_t from;
    std::size_t to;
    int weight;
};

class DirectedGraph {
private:
    std::vector<std::string> names;
    std::unordered_map<std::string, std::size_t> indices;
    std::vector<DirectedEdge> edges;

public:
    void addVertex(const std::string& name);

    void addEdge(
        const std::string& from,
        const std::string& to,
        int weight
    );

    bool hasVertex(const std::string& name) const;

    std::size_t indexOf(const std::string& name) const;

    const std::vector<std::string>& getVertices() const;

    const std::vector<DirectedEdge>& getEdges() const;
};

#endif
