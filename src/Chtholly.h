#ifndef CHTHOLLY_CHTHOLLY_H
#define CHTHOLLY_CHTHOLLY_H

#include <string>

class Chtholly {
public:
    static void runFile(const std::string& path);
    static void runPrompt();

private:
    static void run(const std::string& source);
};

#endif //CHTHOLLY_CHTHOLLY_H
