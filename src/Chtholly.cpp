#include "Chtholly.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"
#include "Transpiler.h"

namespace chtholly {
    void Chtholly::runFile(const std::string &path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << path << std::endl;
            exit(1);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();
        run(source);
    }

    void Chtholly::runPrompt() {
        std::cout << "> ";
        std::string line;
        while (std::getline(std::cin, line)) {
            run(line);
            std::cout << "> ";
        }
    }

    void Chtholly::run(const std::string &source) {
        try {
            chtholly::Lexer lexer(source);
            std::vector<chtholly::Token> tokens = lexer.scanTokens();

            chtholly::Parser parser(tokens);
            std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();

            chtholly::Transpiler transpiler;
            std::string output = transpiler.transpile(statements);

            std::cout << output;
        } catch (const std::exception& e) {
            std::cerr << "An error occurred: " << e.what() << std::endl;
        }
    }
}
