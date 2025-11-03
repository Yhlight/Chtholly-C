#include "Chtholly.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"

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
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }
}
