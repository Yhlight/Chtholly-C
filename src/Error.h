#ifndef CHTHOLLY_ERROR_H
#define CHTHOLLY_ERROR_H

#include <string>

class ErrorReporter {
public:
    static bool hadError;
    static void report(int line, const std::string& message);
    static void reset();
};

#endif // CHTHOLLY_ERROR_H
