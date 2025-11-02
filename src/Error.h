#pragma once

#include <string>

class ErrorReporter {
public:
    static void error(int line, const std::string& message);
    static void report(int line, const std::string& where, const std::string& message);
    static bool hadError;
};
