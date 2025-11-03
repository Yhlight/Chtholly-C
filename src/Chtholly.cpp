#include "Chtholly.h"
#include <iostream>
#include <fstream>
#include <sstream>

void Chtholly::runFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str());
}

void Chtholly::runPrompt() {
    std::string line;
    for (;;) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break;
        }
        run(line);
    }
}

void Chtholly::run(const std::string& source) {
    // For now, just print the source code.
    std::cout << "Executing: " << source << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << "Usage: Chtholly [script]" << std::endl;
        return 64;
    } else if (argc == 2) {
        Chtholly::runFile(argv[1]);
    } else {
        Chtholly::runPrompt();
    }
    return 0;
}
