
#include "Graph.h"
#include "algorithms/Dijkstra.h"
#include "algorithms/AStar.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <cmath>

using namespace std;

double Graph::heuristic(
    const string& a,
    const string& b
) const {
    const City& p = cities.at(a);
    const City& q = cities.at(b);

    return hypot(p.x - q.x, p.y - q.y);
}


bool Graph::hasCity(const string& name) const {
    return cities.count(name) > 0;
}

const vector<pair<string, int>>&
Graph::getNeighbors(const string& city) const {
    return roads.at(city);
}

double Graph::straightLineDistance(
        const string& a,
        const string& b
    ) const {
        return heuristic(a, b);
    }

void Graph::addCity(
        const string& name,
        double x,
        double y
    ) {
        if (!std::isfinite(x) || !std::isfinite(y)) {
            throw std::invalid_argument(
                "City coordinates must be finite."
            );
        }

        if (name.empty() || !isfinite(x) || !isfinite(y))
            throw invalid_argument("Invalid city information.");

        if (cities.count(name))
            throw invalid_argument("City already exists.");

        cities[name] = {x, y};
        roads[name] = {};
    }

void Graph::addRoad(
        const string& a,
        const string& b,
        int distance
    ) {
        if (!cities.count(a) || !cities.count(b))
            throw invalid_argument("Add both cities first.");

        if (a == b)
            throw invalid_argument("Cities must be different.");

        if (distance < 0)
            throw invalid_argument("Distance cannot be negative.");

        if (static_cast<double>(distance) < heuristic(a, b))
            throw invalid_argument(
                "Road is shorter than straight-line distance."
            );

        roads[a].push_back({b, distance});
        roads[b].push_back({a, distance});
    }

bool Graph::updateRoad(
        const std::string& city1,
        const std::string& city2,
        int oldDistance,
        int newDistance
    ) {
        if (!hasCity(city1) || !hasCity(city2)) {
            throw std::invalid_argument(
                "One or both cities do not exist."
            );
        }

        if (city1 == city2) {
            throw std::invalid_argument(
                "A road must connect different cities."
            );
        }

        if (oldDistance < 0 || newDistance < 0) {
            throw std::invalid_argument(
                "Road distances cannot be negative."
            );
        }

        if (static_cast<double>(newDistance) <
            straightLineDistance(city1, city2)) {
            throw std::invalid_argument(
                "New distance is shorter than straight-line distance."
            );
        }

        auto& firstRoads = roads.at(city1);
        auto& secondRoads = roads.at(city2);

        auto first = std::find_if(
            firstRoads.begin(),
            firstRoads.end(),
            [&](const auto& road) {
                return road.first == city2 &&
                    road.second == oldDistance;
            }
        );

        auto second = std::find_if(
            secondRoads.begin(),
            secondRoads.end(),
            [&](const auto& road) {
                return road.first == city1 &&
                    road.second == oldDistance;
            }
        );

        if (first == firstRoads.end() ||
            second == secondRoads.end()) {
            return false;
        }

        first->second = newDistance;
        second->second = newDistance;

        return true;
    }


bool Graph::removeRoad(
        const std::string& city1,
        const std::string& city2,
        int distance
    ) {
        if (!hasCity(city1) || !hasCity(city2)) {
            throw std::invalid_argument(
                "One or both cities do not exist."
            );
        }

        if (city1 == city2) {
            throw std::invalid_argument(
                "A road must connect different cities."
            );
        }

        if (distance < 0) {
            throw std::invalid_argument(
                "Distance cannot be negative."
            );
        }

        auto& firstRoads = roads.at(city1);
        auto& secondRoads = roads.at(city2);

        auto first = std::find_if(
            firstRoads.begin(),
            firstRoads.end(),
            [&](const auto& road) {
                return road.first == city2 &&
                    road.second == distance;
            }
        );

        auto second = std::find_if(
            secondRoads.begin(),
            secondRoads.end(),
            [&](const auto& road) {
                return road.first == city1 &&
                    road.second == distance;
            }
        );

        if (first == firstRoads.end() ||
            second == secondRoads.end()) {
            return false;
        }

        firstRoads.erase(first);
        secondRoads.erase(second);

        return true;
    }

void Graph::displayGraph() 
    const {
        cout << "\n--- Road Network ---\n";

        for (const auto& [city, neighbors] : roads) {
            cout << city << " -> ";

            for (const auto& [next, distance] : neighbors) {
                cout << next << "(" << distance << " km) ";
            }

            cout << '\n';
        }
    }

RouteResult Graph::calculateRoute(
        const string& start,
        const string& destination,
        bool useAStar
    ) const {
        if (useAStar) {
            return aStarShortestPath(
                *this, start, destination
            );
        }

        return dijkstraShortestPath(
            *this, start, destination
        );
    }

void Graph::findRoute(
        const string& start,
        const string& destination,
        bool useAStar
    ) const {
        RouteResult result =
            calculateRoute(start, destination, useAStar);

        cout << "\nAlgorithm: "
            << (useAStar ? "A*" : "Dijkstra") << '\n';

        if (!result.validCities) {
            cout << "City not found!\n";
            return;
        }

        if (!result.found) {
            cout << "No route found!\n";
            cout << "Cities explored: "
                << result.explored << '\n';
            return;
        }

        cout << "Shortest Route: ";

        for (size_t i = 0; i < result.path.size(); ++i) {
            if (i > 0)
                cout << " -> ";

            cout << result.path[i];
        }

        cout << "\nTotal Distance: "
            << result.distance << " km\n";

        cout << "Cities explored: "
            << result.explored << '\n';
    }


std::vector<std::string> Graph::getCityNames() const {
    std::vector<std::string> names;

    names.reserve(cities.size());

    for (const auto& [name, position] : cities) {
        names.push_back(name);
    }

    return names;
}

City Graph::getCity(const std::string& name) const {
    return cities.at(name);
}