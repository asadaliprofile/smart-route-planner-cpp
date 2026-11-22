
#include "NetworkIO.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <string>

using namespace std;

namespace {

// Escape a field using standard CSV quotation rules.
string csvField(const string& value) {
    string result = "\"";

    for (char c : value) {
        if (c == '"')
            result += "\"\"";
        else
            result += c;
    }

    return result + "\"";
}

// Parse one CSV record, including quoted fields.
vector<string> parseCsvLine(const string& line) {
    vector<string> fields;
    string field;

    bool quoted = false;
    bool closed = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (quoted) {
            if (c == '"') {
                if (i + 1 < line.size() &&
                    line[i + 1] == '"') {
                    field += '"';
                    ++i;
                }
                else {
                    quoted = false;
                    closed = true;
                }
            }
            else {
                field += c;
            }
        }
        else if (c == ',') {
            fields.push_back(field);
            field.clear();
            closed = false;
        }
        else if (c == '"' &&
                 field.empty() && !closed) {
            quoted = true;
        }
        else if (c == '"' || closed) {
            throw runtime_error("Malformed CSV field.");
        }
        else {
            field += c;
        }
    }

    if (quoted)
        throw runtime_error("Unterminated CSV quotation.");

    fields.push_back(field);
    return fields;
}


double parseDouble(const string& value) {
    size_t consumed = 0;
    double number = 0.0;

    try {
        number = stod(value, &consumed);
    }
    catch (const exception&) {
        throw runtime_error(
            "Invalid coordinate: " + value
        );
    }

    if (consumed != value.size()) {
        throw runtime_error(
            "Invalid coordinate: " + value
        );
    }

    if (!std::isfinite(number)) {
        throw runtime_error(
            "Coordinate must be finite."
        );
    }

    return number;
}

int parseInteger(const string& value) {
    size_t consumed = 0;
    int number = stoi(value, &consumed);

    if (consumed != value.size())
        throw runtime_error("Invalid road weight.");

    return number;
}

} // namespace

void saveNetwork(
    const Graph& graph,
    const string& filename
) {
    ofstream file(filename);

    if (!file)
        throw runtime_error("Cannot open file for writing.");

    file << setprecision(
        numeric_limits<double>::max_digits10
    );

    file << "type,field1,field2,field3\n";

    vector<string> cities = graph.getCityNames();
    sort(cities.begin(), cities.end());

    // Write each city and its coordinates.
    for (const string& name : cities) {
        City city = graph.getCity(name);

        file << "CITY,"
             << csvField(name) << ","
             << city.x << ","
             << city.y << '\n';
    }

    // Write each undirected road only once.
    for (const string& name : cities) {
        for (const auto& [neighbor, weight] :
             graph.getNeighbors(name)) {

            if (name < neighbor) {
                file << "ROAD,"
                     << csvField(name) << ","
                     << csvField(neighbor) << ","
                     << weight << '\n';
            }
        }
    }

    file.flush();

    if (!file)
        throw runtime_error("Failed while writing CSV.");
}

void loadNetwork(
    Graph& graph,
    const string& filename
) {
    ifstream file(filename);

    if (!file)
        throw runtime_error("Cannot open file for reading.");

    // Do not replace the original graph until
    // the entire file has been validated.
    Graph temporary;

    struct RoadRecord {
        string from;
        string to;
        int weight;
        size_t lineNumber;
    };

    vector<RoadRecord> pendingRoads;

    string line;
    size_t lineNumber = 0;

    if (!getline(file, line))
        throw runtime_error("CSV file is empty.");

    ++lineNumber;

    // Handle files using Windows CRLF line endings.
    if (!line.empty() && line.back() == '\r')
        line.pop_back();

    if (line != "type,field1,field2,field3")
        throw runtime_error("Invalid CSV header.");

    while (getline(file, line)) {
        ++lineNumber;

        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line.empty())
            continue;

        try {
            vector<string> fields = parseCsvLine(line);

            if (fields.size() != 4)
                throw runtime_error("Expected four fields.");

            if (fields[0] == "CITY") {
                temporary.addCity(
                    fields[1],
                    parseDouble(fields[2]),
                    parseDouble(fields[3])
                );
            }
            else if (fields[0] == "ROAD") {
                pendingRoads.push_back({
                    fields[1],
                    fields[2],
                    parseInteger(fields[3]), 
                    lineNumber
                });
            }
            else {
                throw runtime_error("Unknown record type.");
            }
        }
        catch (const exception& e) {
            throw runtime_error(
                "CSV line " + to_string(lineNumber) +
                ": " + e.what()
            );
        }
    }

    if (file.bad())
        throw runtime_error("Failed while reading CSV.");

    // Add roads only after all cities have been loaded.
    

    for (const auto& road : pendingRoads) {
        try {
            temporary.addRoad(
                road.from,
                road.to,
                road.weight
            );
        }
        catch (const exception& e) {
            throw runtime_error(
                "CSV line " +
                to_string(road.lineNumber) +
                ": " + e.what()
            );
        }
    }


    // Commit only after the entire load succeeds.
    graph = std::move(temporary);
}
