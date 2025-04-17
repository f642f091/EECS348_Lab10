#include <iostream>
#include <fstream>
#include <string>
#include "calculator.hpp"

int main() {
    std::ifstream file("input.txt"); // open input file
    std::string line;                // line buffer

    if (!file.is_open()) {
        std::cerr << "Error opening file.\n"; // error msg if input.txt missing
        return 1;
    }

    // read each line from file
    while (getline(file, line)) {
        if (isValidDouble(line)) {
            std::cout << "Valid: " << line << std::endl;
            std::string result = addDoubles(line, "-123.456"); // add to fixed value
            std::cout << line << " + (-123.456) = " << result << "\n\n";
        } else {
            std::cout << "Invalid: " << line << "\n\n"; // not a valid double
        }
    }

    file.close(); // clean up
    return 0;
}
