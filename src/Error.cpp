#include "Error.h"
#include <iostream>

bool ErrorReporter::hadError = false;
bool ErrorReporter::hadRuntimeError = false;

void ErrorReporter::error(int line, const std::string& message) {
    report(line, "", message);
}

void ErrorReporter::runtimeError(int line, const std::string& message) {
    report(line, " [runtime]", message);
    hadRuntimeError = true;
}

void ErrorReporter::report(int line, const std::string& where, const std::string& message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError = true;
}

void ErrorReporter::reset() {
    hadError = false;
    hadRuntimeError = false;
}
