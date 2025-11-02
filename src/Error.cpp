#include "Error.h"
#include <iostream>

bool ErrorReporter::hadError = false;

void ErrorReporter::error(int line, const std::string& message) {
    report(line, "", message);
}

void ErrorReporter::report(int line, const std::string& where, const std::string& message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError = true;
}
