#pragma once

#include <string>
#include "../frontend/Token.h"

class ErrorReporter {
public:
    static void error(int line, const std::string& message);
    static void error(const Token& token, const std::string& message);
    static void report(int line, const std::string& where, const std::string& message);
    static void reset();
    static bool hadError;
};
