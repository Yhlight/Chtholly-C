#pragma once

#include <string>

class ErrorReporter {
public:
    static bool hadError;

    static void report(int line, const std::string& where, const std::string& message);
    static void error(int line, const std::string& message);
    static void reset();
};
