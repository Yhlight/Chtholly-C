#include "Error.h"
#include <iostream>

bool ErrorReporter::hadError = false;

void ErrorReporter::error(int line, const std::string& message) {
    report(line, "", message);
}

void ErrorReporter::error(const Token& token, const std::string& message) {
    if (token.type == TokenType::END_OF_FILE) {
        report(token.line, " at end", message);
    } else {
        report(token.line, " at '" + token.lexeme + "'", message);
    }
}

void ErrorReporter::report(int line, const std::string& where, const std::string& message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError = true;
}

void ErrorReporter::reset() {
    hadError = false;
}
