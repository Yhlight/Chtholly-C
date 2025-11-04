#include "Chtholly.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << "Usage: chtholly [script]" << std::endl;
        return 64;
    } else if (argc == 2) {
        Chtholly::runFile(argv[1]);
    } else {
        Chtholly::runPrompt();
    }
    return 0;
}
