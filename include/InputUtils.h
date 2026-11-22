#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include <istream>
#include <ostream>
#include <string>

// Reads a valid integer.
// Returns false if the input stream reaches EOF.
bool readInteger(
    std::istream& input,
    std::ostream& output,
    const std::string& prompt,
    int& value
);

// Reads nonempty text, including spaces.
// Returns false if the input stream reaches EOF.
bool readText(
    std::istream& input,
    std::ostream& output,
    const std::string& prompt,
    std::string& value
);

bool readDouble(
    std::istream& input,
    std::ostream& output,
    const std::string& prompt,
    double& value
);

#endif
