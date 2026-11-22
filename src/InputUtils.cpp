
#include "InputUtils.h"

#include <sstream>
#include <string>
#include <cmath>
#include <limits>

using namespace std;

namespace {

// Remove whitespace from both ends of a string.
string trim(const string& text) {
    const string whitespace = " \t\n\r\f\v";

    size_t first = text.find_first_not_of(whitespace);

    if (first == string::npos)
        return "";

    size_t last = text.find_last_not_of(whitespace);

    return text.substr(first, last - first + 1);
}

} // namespace

bool readInteger(
    istream& input,
    ostream& output,
    const string& prompt,
    int& value
) {
    string line;

    while (true) {
        output << prompt;

        if (!getline(input >> ws, line))
            return false;

        istringstream parser(line);
        int parsedValue;

        if (parser >> parsedValue) {
            parser >> ws;

            if (parser.eof()) {
                value = parsedValue;
                return true;
            }
        }

        output << "Invalid input. "
               << "Please enter a whole number.\n";
    }
}

bool readText(
    istream& input,
    ostream& output,
    const string& prompt,
    string& value
) {
    string line;

    while (true) {
        output << prompt;

        if (!getline(input >> ws, line))
            return false;

        line = trim(line);

        if (!line.empty()) {
            value = line;
            return true;
        }

        output << "Input cannot be empty.\n";
    }
}


bool readDouble(
    std::istream& input,
    std::ostream& output,
    const std::string& prompt,
    double& value
) {
    std::string line;

    while (true) {
        output << prompt;

        if (!std::getline(input >> std::ws, line))
            return false;

        std::istringstream parser(line);
        double parsedValue;

        if (parser >> parsedValue) {
            parser >> std::ws;

            if (parser.eof() &&
                std::isfinite(parsedValue)) {
                value = parsedValue;
                return true;
            }
        }

        output << "Invalid input. "
               << "Please enter a finite number.\n";
    }
}