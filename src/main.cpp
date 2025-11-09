#include "Chtholly.h"
#include <iostream>

int main(int argc, char* argv[]) {
    chtholly::Chtholly chtholly;
    if (argc > 2) {
        std::cerr << "Usage: chtholly [script]" << std::endl;
        return 64;
    } else if (argc == 2) {
        return chtholly.runFile(argv[1]);
    } else {
        chtholly.runPrompt();
    }
    return 0;
}
