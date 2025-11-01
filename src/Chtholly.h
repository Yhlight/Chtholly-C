#pragma once

#include <string>

class Chtholly {
public:
    static void runFile(const std::string& path);
    static void runPrompt();

private:
    static void run(const std::string& source);
};
