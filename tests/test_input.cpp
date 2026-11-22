
#include "InputUtils.h"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int main() {
    int passed = 0;
    int failed = 0;

    auto check = [&](bool condition, const string& name) {
        if (condition) {
            cout << "[PASS] " << name << '\n';
            ++passed;
        }
        else {
            cout << "[FAIL] " << name << '\n';
            ++failed;
        }
    };

    // Test 1: Valid integer.
    {
        istringstream input("12\n");
        ostringstream output;
        int value = 0;

        bool success =
            readInteger(input, output, "Number: ", value);

        check(
            success && value == 12,
            "Accepts a valid integer"
        );
    }

    // Test 2: Reject invalid text, then retry.
    {
        istringstream input("abc\n15\n");
        ostringstream output;
        int value = 0;

        bool success =
            readInteger(input, output, "Number: ", value);

        check(
            success &&
            value == 15 &&
            output.str().find("Invalid input") !=
                string::npos,
            "Recovers from invalid integer input"
        );
    }

    // Test 3: Reject trailing characters.
    {
        istringstream input("12abc\n20\n");
        ostringstream output;
        int value = 0;

        bool success =
            readInteger(input, output, "Number: ", value);

        check(
            success &&
            value == 20 &&
            output.str().find("Invalid input") !=
                string::npos,
            "Rejects integers with trailing text"
        );
    }

    // Test 4: Accept names containing spaces.
    {
        istringstream input("New York\n");
        ostringstream output;
        string value;

        bool success =
            readText(input, output, "City: ", value);

        check(
            success && value == "New York",
            "Accepts multiword text"
        );
    }

    // Test 5: Handle an empty input stream.
    {
        istringstream input("");
        ostringstream output;
        int value = 0;

        bool success =
            readInteger(input, output, "Number: ", value);

        check(
            !success,
            "Handles end-of-file gracefully"
        );
    }

    // Test 6: Read integer after legacy token extraction.
    {
        istringstream input("A\n25\n");
        ostringstream output;

        string name;
        input >> name;

        int value = 0;
        bool success =
            readInteger(input, output, "Weight: ", value);

        check(
            success && value == 25,
            "Works after existing stream extraction"
        );
    }


    // Test 7: Accept a positive decimal.
    {
        istringstream input("2.75\n");
        ostringstream output;
        double value = 0;

        bool success = readDouble(
            input, output, "Coordinate: ", value
        );

        check(
            success && value == 2.75,
            "Accepts positive decimal coordinates"
        );
    }

    // Test 8: Accept a negative decimal.
    {
        istringstream input("-4.5\n");
        ostringstream output;
        double value = 0;

        bool success = readDouble(
            input, output, "Coordinate: ", value
        );

        check(
            success && value == -4.5,
            "Accepts negative decimal coordinates"
        );
    }

    // Test 9: Reject text and retry.
    {
        istringstream input("abc\n3.5\n");
        ostringstream output;
        double value = 0;

        bool success = readDouble(
            input, output, "Coordinate: ", value
        );

        check(
            success &&
            value == 3.5 &&
            output.str().find("Invalid input") !=
                string::npos,
            "Recovers from invalid decimal input"
        );
    }

    // Test 10: Reject trailing characters.
    {
        istringstream input("12abc\n7.25\n");
        ostringstream output;
        double value = 0;

        bool success = readDouble(
            input, output, "Coordinate: ", value
        );

        check(
            success &&
            value == 7.25 &&
            output.str().find("Invalid input") !=
                string::npos,
            "Rejects decimal with trailing text"
        );
    }

    // Test 11: Reject a non-finite value.
    {
        istringstream input("nan\n5.5\n");
        ostringstream output;
        double value = 0;

        bool success = readDouble(
            input, output, "Coordinate: ", value
        );

        check(
            success &&
            value == 5.5 &&
            output.str().find("Invalid input") !=
                string::npos,
            "Rejects NaN coordinates"
        );
    }

    // Test 12: Handle EOF.
    {
        istringstream input("");
        ostringstream output;
        double value = 0;

        bool success = readDouble(
            input, output, "Coordinate: ", value
        );

        check(
            !success,
            "Handles EOF when reading coordinates"
        );
    }


    cout << "\n===== INPUT TEST SUMMARY =====\n";
    cout << "Passed: " << passed << '\n';
    cout << "Failed: " << failed << '\n';

    return failed == 0 ? 0 : 1;
}
