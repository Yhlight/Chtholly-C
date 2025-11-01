#include "Error.h"
#include <iostream>

bool ErrorReporter::hadError = false;

void ErrorReporter::report(int line, const std::string& message) {
    std::cerr << "[line " << line << "] Error: " << message << std::endl;
    hadError = true;
}

void ErrorReporter::reset() {
    hadError = false;
}
