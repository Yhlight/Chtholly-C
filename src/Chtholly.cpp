#include "Chtholly.h"
#include <iostream>
#include <fstream>
#include <sstream>

void Chtholly::runFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Could not open file: " << path << std::endl;
        exit(74);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str());
}

void Chtholly::runPrompt() {
    std::string line;
    for (;;) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        run(line);
    }
}

void Chtholly::run(const std::string& source) {
    // For now, just print the source
    std::cout << "Executing: " << source << std::endl;
}
