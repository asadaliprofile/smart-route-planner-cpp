
#ifndef GRAPH_H
#define GRAPH_H

#include <cstddef>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct City {
    double x;
    double y;
};

struct RouteResult {
    bool validCities = false;
    bool negativeCycle = false;
    bool found = false;
    long long distance = 0;

    std::vector<std::string> path;
    std::size_t explored = 0;
};

class Graph {
private:
    std::unordered_map<std::string, City> cities;

    std::unordered_map<
        std::string,
        std::vector<std::pair<std::string, int>>
    > roads;

    double heuristic(
        const std::string& a,
        const std::string& b
    ) const;

public:

    bool hasCity(const std::string& name) const;

    std::vector<std::string> getCityNames() const;
    const std::vector<std::pair<std::string, int>>&
    getNeighbors(const std::string& city) const;

    City getCity(const std::string& name) const;

    double straightLineDistance(
        const std::string& a,
        const std::string& b
    ) const;



    void addCity(
        const std::string& name,
        double x,
        double y
    );

    void addRoad(
        const std::string& a,
        const std::string& b,
        int distance
    );

    bool updateRoad(
        const std::string& city1,
        const std::string& city2,
        int oldDistance,
        int newDistance
    );

    bool removeRoad(
        const std::string& city1,
        const std::string& city2,
        int distance
    );

    void displayGraph() const;

    RouteResult calculateRoute(
        const std::string& start,
        const std::string& destination,
        bool useAStar
    ) const;

    void findRoute(
        const std::string& start,
        const std::string& destination,
        bool useAStar
    ) const;
};

#endif
